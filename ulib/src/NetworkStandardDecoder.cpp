/*
* NetworkStandardDecoder.cpp
*
*  Created on: 10.06.2012
*      Author: Endre Bock
*/

#include <NetworkStandardDecoder>
#include <Datablock>
#include <NetworkConnection>
#include <NetEvent>

using namespace u;

u::NetworkStandardDecoder::NetworkStandardDecoder() : NetworkDecoder()
{
	_ready4Send = false;
	_wait4Ack   = false;
	_nextId     = 0;
}

u::NetworkStandardDecoder::~NetworkStandardDecoder()
{
}

void u::NetworkStandardDecoder::destroy()
{
	delete (NetworkStandardDecoder*)this;
}

String u::NetworkStandardDecoder::toString()
{
	String out = "[ " + className() + "]";
	return out;
}

String u::NetworkStandardDecoder::className()
{
	return "u::NetworkStandardDecoder";
}

// called in lock!!
void u::NetworkStandardDecoder::decodeData(ByteArray* buffer)
{
	bool readComplete = false; // flag: block complete read and clean buffer after read
	Datablock* frame;
	uint8 byte, id, type;
	uint64 readBytes;

	trace(
		className()+"::decodeData: Incomming data "+int2string(buffer->available())
	);
	trace(buffer->toString());

	readBytes = 0;
	uint8 cnt = 0;
	while(cnt++ == 0)// ein loop nur
	{
		buffer->lock();
		buffer->position(0);
		// 3 is the smallest package
		if(buffer->available() < 3)
		{
			readComplete = true;
			buffer->unlock();
			continue; // skip rest of on-loop
		}

		byte = buffer->readByte();
		readBytes ++;
		id = byte >> 7;

		if(id != _nextId)
		{
			error(
				className()+"::decodeData: Incorrect id received. Id was " +
				int2string(id) + " but expected is " + int2string(_nextId) + "."
			);
			//todo: Resync handling.
			abortConnection(buffer);
			readComplete = true;
			continue; // skip rest
		}

		type = byte & 0x7F;
		switch(type)
		{
			case TYPE_HANDSHAKE:
			{
				trace(className()+"::decodeData: Received HANDSHAKE");
				// check length for HANDSHAKE
				if(buffer->available() < 4)
				{
					trace(className()+"::decodeData: HANDSHAKE: package incomplete.");
					break;
				}

				int16 version = buffer->readInt(2, TYPE_UINT);
				int8 l = checkEndBytes(buffer);
				readBytes += l;
				if(l != 2 || !doHandshake(version))
				{
					trace(
						className()+"::decodeData: Incorrect block length or"
						+" handshake failed."
					);
					unlock();
					sendCloseConfirmed();
					lock();
					deleteTimeout(wordCloseConfirmed);
					abortConnection(buffer);
				}
				readComplete = true;
			} break;
			case TYPE_HACK:
			{
				deleteTimeout(wordHandshake);
				flipNextId();
				trace(className()+"::decodeData: Handshake complete.");

				int8 l = checkEndBytes(buffer);
				readBytes += l;
				if(l != 2)
				{
					abortConnection(buffer);
				}
				else
				{
					_ready4Send = true;
					_con->doReady();
				}
				readComplete = true;
			} break;
			case TYPE_CLOSE_CONFIRMED:
			{
				trace(className()+"::decodeData: Close confirmation received.");
				deleteTimeout(wordClose);
				int8 l = checkEndBytes(buffer);
				readBytes += l;
				bool ready4Send = _ready4Send;
				if(l != 2 || !ready4Send)
				{
					error(className()+"::decodeData() Handshake failed or datablock wrong.");
					deleteTimeout(wordHandshake);
					abortConnection(buffer);
				}
				else
				{
					sendAck();
					Callback cb(this, cb_cast(&NetworkStandardDecoder::doClosed));
					ThreadSystem::create(&cb);
				}

				readComplete = true;
			} break;
			case TYPE_CLOSE:
			{
				trace(className()+"::decodeData: Incomming close.");

				int8 l = checkEndBytes(buffer);
				readBytes += l;
				if(l != 2) abortConnection(buffer);
				else
				{
					_con->doCloseRequired();
				}

				readComplete = true;
			} break;
			case TYPE_ACK:
			{
				trace(className()+"::decodeData: Incomming ACK.");
				int8 l = checkEndBytes(buffer);
				readBytes += l;
				if(l != 2)
					abortConnection(buffer);
				else
					progressAck();

				readComplete = true;
			} break;

			case TYPE_T_INIT:
			{
				if (buffer->available() < 10) {
					// to small/incomplete
					break;
				}
				trace(className()+"::decodeData: Incomming transfrer init.");
				uint64 id = buffer->readInt(8, TYPE_UINT);
				int8 l = checkEndBytes(buffer);

				if(l != 2)
					abortConnection(buffer); // TODO resync handling
				else
				{
					_con->doTransferInit(id);
					sendAck();
				}

				readComplete = true;
			} break;

			case TYPE_T_READY:
			{
				if (buffer->available() < 10) {
					// to small/incomplete
					break;
				}
				trace(className()+"::decodeData: Incomming transfer ready.");
				uint64 id = buffer->readInt(8, TYPE_UINT);
				int8 l = checkEndBytes(buffer);

				if(l != 2)
					abortConnection(buffer); // TODO resync handling
				else
				{
					sendAck();
					_con->doTransfer(id);
				}

				readComplete = true;
			} break;

			case TYPE_T_DATA:
			{
				trace(className()+"::decodeData: Incomming data.");
				if (buffer->available() < 11) {
					// to small/incomplete
					trace(className()+"::decodeData: Incomming data. Sub header to small");
					break;
				}
				uint64 id = buffer->readInt(8, TYPE_UINT);
				uint32 length = buffer->readInt(3, TYPE_UINT);
				readBytes += 11;
				if(buffer->available() < length + 2/*end bytes*/)
				{
					// to small/incomplete
					trace(className()+"::decodeData: Incomming data. Data incomplete");
					break;
				}
				ByteArray data;
				readBytes += buffer->readBytes(&data, length);
				int8 l = checkEndBytes(buffer);

				if(l != 2)
					abortConnection(buffer); // TODO resync handling
				else
				{
					sendAck();
					_con->doReceivedData(id, &data);
				}
				readComplete = true;
			} break;

			default:
			{
				error(
					className()+"::decodeData: Unkown package type " + int2string(type)
					+ " (byte:" + int2string(byte) + ") with id " + int2string(id) + "."
				);
				abortConnection(buffer);
				readComplete = true;
			} break;
		}

		// rollback if sequence not complete
		if(readComplete != true)
		{
			trace (int2string(buffer->position()) + " - " +int2string(readBytes));
			buffer->position(buffer->position() - readBytes);
			readBytes = 0;
			trace("Position reset to: "+int2string(buffer->position()));
		}
		// clean
		buffer->splice(0, buffer->position());
		buffer->unlock();
	}
}

int64 u::NetworkStandardDecoder::checkEndBytes(ByteArray* buffer)
{
	uint8 b1, b2;
	b1 = buffer->readByte();
	b2 = buffer->readByte();
	if(b1 != 0xFF || b2 != 0xEB)
	{
		error(className()+"::checkEndBytes: incorrect byte sequence "+int2string(b1)
				+", "+int2string(b2)
		);
		buffer->position(buffer->position() - 2);
		return 0;
	}
	return 2;
}

void u::NetworkStandardDecoder::abortConnection(ByteArray* buffer)
{
	error(className()+": Abort connection.");
	buffer->position(buffer->length());
	Callback cb(this, cb_cast(&NetworkStandardDecoder::doClosed));
	ThreadSystem::create(&cb);
}

void u::NetworkStandardDecoder::startHandshake(Object* arg)
{
	trace(className()+": Start HANDSHAKE.");
	ByteArray data;
	data.writeByte(TYPE_HANDSHAKE); // HANDSHAKE
	data.writeInt(_version, 2, true);
	addEndBytes(&data);

	Callback cbRun(this, cb_cast(&NetworkStandardDecoder::doClosed));
	Callback cbAbort(
		this
		, cb_cast(&NetworkStandardDecoder::removeTimeout)
		, &wordHandshake
	);
	lock();
	_timeoutList[wordHandshake] = new Timeout(_con->timeout, &cbRun, &cbAbort);
	send(&data);
	unlock();
}

// called in lock
bool u::NetworkStandardDecoder::doHandshake(uint16 version)
{
	ByteArray* data;

	if(version != _version)
	{
		trace(className()+"::doHandshake: Incorrect protocol ("
				+int2string(version)+" <> "+int2string(_version)+")"
		);
		return false;
	}

	trace(className()+": Send HACK");
	data = new ByteArray();
	data->writeByte(TYPE_HACK); // HACK
	addEndBytes(data);
	send(data);
	flipNextId();
	data->destroy();
	_ready4Send = true;
	return true;
}

void u::NetworkStandardDecoder::addEndBytes(ByteArray* data)
{
	data->writeByte(0xFF);
	data->writeByte(0xEB);
}

void u::NetworkStandardDecoder::sendClose()
{
	ByteArray data;
	uint8 command = TYPE_CLOSE;

	addIdToCommandByte(command);

	data.writeByte(command);
	addEndBytes(&data);

	Callback cbRun(
		this
		, cb_cast(&NetworkStandardDecoder::doCloseTimeout)
	);
	Callback cbAbort(
		this
		, cb_cast(&NetworkStandardDecoder::removeTimeout)
		, &wordClose
	);
	lock();
	_timeoutList[wordClose] = new Timeout(_con->timeout, &cbRun, &cbAbort);
	send(&data);
	unlock();
}

void u::NetworkStandardDecoder::doCloseTimeout(Object* arg)
{
	lock();
	deleteTimeout(wordClose);
	error(
		className()
		+ "::doCloseTimeout(): No answer of close, try to send close confirmation."
	);
	_con->doCloseRequired();
	unlock();
}

void u::NetworkStandardDecoder::sendCloseConfirmed()
{
	uint8 command = TYPE_CLOSE_CONFIRMED;
	addIdToCommandByte(command);

	ByteArray data;
	data.writeByte(command);
	addEndBytes(&data);

	Callback cbRun(
		this
		, cb_cast(&NetworkStandardDecoder::doClosed)
	);
	Callback cbAbort(
		this
		, cb_cast(&NetworkStandardDecoder::removeTimeout)
		, &wordCloseConfirmed
	);
	lock();
	_timeoutList[wordCloseConfirmed] = new Timeout(_con->timeout, &cbRun, &cbAbort);
	send(&data);
	unlock();
}

void u::NetworkStandardDecoder::addIdToCommandByte(uint8& command)
{
	if(_nextId == 1) command += 0x80;
}

void u::NetworkStandardDecoder::flipNextId()
{
	_nextId = (_nextId == 1) ? 0 : 1;
}

bool u::NetworkStandardDecoder::isReady()
{
	return _ready4Send;
}

void u::NetworkStandardDecoder::sendAck()
{
	uint8 command = TYPE_ACK;
	addIdToCommandByte(command);
	ByteArray data;
	data.writeByte(command);
	addEndBytes(&data);
	send(&data);
	flipNextId();
}

// called in lock
void u::NetworkStandardDecoder::progressAck()
{
	/**
	 * Generic actions: ACK flips the id.
	 */
	flipNextId();

	/**
	 * Action for ACk after CLOSE_CONFIRMED.
	 */
	_con->lock();
	bool connectionCloseAck = _con->isClose() == true;
	_con->unlock();

	if(connectionCloseAck == true)
	{
		deleteTimeout(wordCloseConfirmed);
		Callback cb(this, cb_cast(&NetworkStandardDecoder::doClosed));
		ThreadSystem::create(&cb);
		return;
	}
}

void u::NetworkStandardDecoder::deleteTimeout(String word)
{
	if(_timeoutList.hasKey(word))
	{
		Timeout* timeout = (Timeout*)((Object*)_timeoutList[word]);
		timeout->stop(); // destroy and delete via abort function call
	}
}

void u::NetworkStandardDecoder::sendInit(uint64 id)
{
	uint8 command = TYPE_T_INIT;
	addIdToCommandByte(command);
	ByteArray data;
	data.writeByte(command);
	data.writeUInt(id, 8, true);
	addEndBytes(&data);
	send(&data);
}

void u::NetworkStandardDecoder::sendTransferReady(uint64 id)
{
	uint8 command = TYPE_T_READY;
	addIdToCommandByte(command);
	ByteArray data;
	data.writeByte(command);
	data.writeUInt(id, 8, true);
	addEndBytes(&data);
	send(&data);
}

void u::NetworkStandardDecoder::sendData(uint64 id, ByteArray* data)
{
	uint8 command = TYPE_T_DATA;
	addIdToCommandByte(command);
	ByteArray outData;
	outData.writeByte(command);
	outData.writeUInt(id, 8, true);
	outData.writeUInt(data->available(), 3, true); // length of data
	outData.writeBytes(*data);
	addEndBytes(&outData);
	send(&outData);
}

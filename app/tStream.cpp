

#include <ByteStream>

namespace u
{
	void programExit();

	class tStream : public Object
	{
		public:
			tStream(Vector<String> arg) : Object()
			{
				ByteStream stream(10);
				ByteArray  source, target;
				int64      wrote, read;

				source.writeBytes("0123456789ABCDEFGHIJKLMNOPQRST", 30);

				trace(
						"Before write\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				source.position(0);
				wrote = stream.writeData(source, 5);

				trace(
						"Wrote " + int2string(wrote) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				read = stream.readData(&target, 3);
				trace(
						"Read " + int2string(read) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);


				wrote = stream.writeData(source);
				trace(
						"Wrote " + int2string(wrote) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				read = stream.readData(&target, 8);
				trace(
						"Read " + int2string(read) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				wrote = stream.writeData(source);
				wrote += stream.writeData(source);
				read  = stream.readData(&target);
				trace(
						"Wrote " + int2string(wrote) + " bytes\n"
						+ "Read " + int2string(read) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				wrote = stream.writeData(source);
				wrote += stream.writeData(source);
				read  = stream.readData(&target);
				read  += stream.readData(&target);
				trace(
						"Wrote " + int2string(wrote) + " bytes\n"
						+ "Read " + int2string(read) + " bytes\n"
						+ source.toString()
						+ "\n"
						+ stream.toString()
						+ "\n"
						+ target.toString()
						+ "\n\n"
				);

				programExit();
			}


			String className()
			{ return "u::tStream"; }
	};
}

#define APPLICATION tStream

#include <Application>

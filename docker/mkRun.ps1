#
# Make run container
#

set BASEDIR $(Split-Path $myInvocation.MyCommand.Path)

copy -v "$BASEDIR/../warp10server/warp10*" "$BASEDIR/run"
docker build -t enbock/warp10server:latest "$BASEDIR/run"

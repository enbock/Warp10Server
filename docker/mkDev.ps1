#
# Make dev container
#

set BASEDIR $(Split-Path $myInvocation.MyCommand.Path)

docker build -t enbock/warp10server-dev:latest "$BASEDIR/dev"

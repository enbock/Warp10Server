#
# Run dev container
#

set BASEDIR $(Split-Path $myInvocation.MyCommand.Path)

docker rm -f W10S
docker run -itd --name W10S enbock/warp10server:latest

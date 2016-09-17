#
# Run dev container
#

set BASEDIR $(Split-Path $myInvocation.MyCommand.Path)

docker rm -f W10SDev
docker run -itd -v "${BASEDIR}\..:/src" -v "${env:USERPROFILE}:/root" -w "/src" --name W10SDev enbock/warp10server-dev

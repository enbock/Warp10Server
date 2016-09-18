#
# Run dev container
#

set BASEDIR $(Split-Path $myInvocation.MyCommand.Path)

docker rm -f W10SRun
docker run -itd -v "${BASEDIR}\..:/src" -v "${env:USERPROFILE}:/root" -w "/src" --name W10SRun busybox /src/warp10server/warp10server

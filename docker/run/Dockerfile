#
# Docker for run the server
# Author: Endre Boock <dev@tibock.de>
#

FROM busybox

COPY warp10server /app/warp10server
COPY warp10admin /app/warp10admin

VOLUME ["/app"]

ENTRYPOINT []
CMD ["/app/warp10server"]

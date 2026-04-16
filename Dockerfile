FROM ubuntu:24.04 AS build

# Snapshot base-image lib filenames BEFORE installing anything,
# so we can later skip copying libs the runtime stage already has.
RUN find /lib /usr/lib -maxdepth 3 -name '*.so*' -printf '%f\n' \
  | sort -u > /tmp/base-libs.txt

RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
  apt-get -qq install --no-install-recommends \
  build-essential \
  libwebkitgtk-6.0-dev

COPY . /src
WORKDIR /src

# Build the binary, then export only the shared libraries it links to
# that are NOT already shipped in the runtime base image.
RUN make webkit-content-filter-validator && \
  mkdir /export && \
  ldd webkit-content-filter-validator \
  | awk '/=>/ && $3 ~ /^\// {print $3}' \
  | while read lib; do \
  name=$(basename "$lib"); \
  grep -qFx "$name" /tmp/base-libs.txt \
  || cp -L "$lib" /export/; \
  done


FROM ubuntu:24.04

COPY --from=build /export/ /usr/local/lib/
COPY --from=build /src/webkit-content-filter-validator /usr/local/bin/

# Refresh /etc/ld.so.cache so the dynamic linker sees the libs we just added.
RUN ldconfig

WORKDIR /tmp
USER 1001:1001

CMD ["/usr/local/bin/webkit-content-filter-validator"]

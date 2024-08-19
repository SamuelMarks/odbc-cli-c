FROM alpine:latest AS builder

RUN apk add --no-cache \
    cmake \
    curl \
    gcc \
    git \
    libarchive-tools \
    libltdl \
    linux-headers \
    make \
    musl-dev \
    unixodbc-dev

WORKDIR /src/odbc-ini-gen
ADD https://github.com/SamuelMarks/odbc-ini-gen/archive/refs/heads/master.zip .
RUN bsdtar xvf master.zip --strip-components=1 && \
    cmake -S . -B build_alpine && \
    cmake --build build_alpine && \
    cmake --install build_alpine --prefix /opt/odbc-ini-gen && \
    printf 'Installed odbc-ini-gen %s' "$(/opt/odbc-ini-gen/bin/odbc_ini_gen --version)"

WORKDIR /src/odbc-cli-c
COPY . .
RUN cmake -S . -B build_alpine && \
    cmake --build build_alpine && \
    cmake --install build_alpine --prefix /opt/odbc-cli-c && \
    printf 'Installed %s' "$(/opt/odbc-cli-c/bin/odbc_cli_c --version)"

#RUN /usr/bin/ninja --version
#RUN export CMAKE_MAKE_PROGRAM="/usr/bin/make" CMAKE_C_COMPILER="/usr/bin/cc" && \
#    git clone --depth=1 --single-branch https://github.com/microsoft/vcpkg /src/vcpkg && \
#    /src/vcpkg/bootstrap-vcpkg.sh -disableMetrics && \
#    -DCMAKE_TOOLCHAIN_FILE='/src/vcpkg/scripts/buildsystems/vcpkg.cmake' \

FROM alpine AS runner

ENV DEFAULT_DB_DRIVER="PostgreSQL UNICODE"

RUN apk add --no-cache libltdl unixodbc psqlodbc wait4x curl jq

COPY --from=builder /opt/odbc-cli-c /opt/odbc-cli-c
COPY --from=builder /opt/odbc-ini-gen/bin/odbc_ini_gen /usr/local/bin/

RUN /usr/local/bin/odbc_ini_gen --infer-all -o /etc/odbcinst.ini && \
    printf '[Default]\nDriver = %s\n' "${DEFAULT_DB_DRIVER}" > /etc/odbc.ini

ENTRYPOINT ["/opt/odbc-cli-c/bin/odbc_cli_c"]

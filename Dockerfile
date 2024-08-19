FROM alpine:latest AS builder

RUN apk add --no-cache gcc musl-dev make cmake linux-headers git
WORKDIR /src/odbc-cli-c
COPY . .
RUN cmake -S . -B build_alpine && \
    cmake --build build_alpine && \
    cmake --install build_alpine --prefix /opt/odbc-cli-c && \
    printf 'Installed %s' "$(/opt/odbc-cli-c/bin/odbc_cli_c --version)"

FROM alpine AS runner
# FROM busybox:musl AS runner
# FROM scratch AS runner
# FROM gcr.io/distroless/static-debian12 AS runner

COPY --from=builder /opt/odbc-cli-c /opt/odbc-cli-c
ENTRYPOINT ["/opt/odbc-cli-c/bin/odbc_cli_c"]

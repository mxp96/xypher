# Xypher Docker

## Quick Start

```bash
docker-compose up -d
docker-compose exec xypher bash
```

Inside container:
```bash
xypc examples/hello.xyp -o output/hello
./output/hello
```

## Build Image

```bash
docker build -f docker/Dockerfile -t xypher:latest .
```

## Run

```bash
docker run -it -v $(pwd)/examples:/workspace/examples xypher:latest
```

## Compile with Docker

```bash
docker run --rm -v $(pwd):/workspace xypher:latest \
  xypc /workspace/examples/hello.xyp -o /workspace/output/hello
```

## See Also

- [../INSTALLATION.md](../INSTALLATION.md) - Native installation (alternative to Docker)
- [../BUILDING.md](../BUILDING.md) - Build instructions
- [../QUICKSTART.md](../QUICKSTART.md) - Getting started guide
- [../README.md](../README.md) - Project overview


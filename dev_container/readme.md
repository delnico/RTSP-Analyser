
# Arch dependencies

```bash
sudo pacman -S nvidia-container-toolkit
```

# Nvidia Runtime as default

## "default-runtime": "nvidia"

Add 

```json
"default-runtime": "nvidia"
```

to "default-runtime": "nvidia"

```json
{
  "runtimes": {
    "nvidia": {
      "path": "nvidia-container-runtime",
      "runtimeArgs": []
    }
  },
  "default-runtime": "nvidia"
}
```

## Run it

```bash
docker-compose up -d --build rtspanalyser-dev
```

## Check if exec on Nvidia GPU

```bash
docker exec -it rtspanalyser-dev bash


nvidia-smi
```

# Running Meshlab in a Docker container

After built meshlab in the container, you can run it, but you'll need to give it access to the X server. You can do this by running the following command in the host machine:

```bash
xhost +local:* # or xhost +local:docker
```

You can disable access to the X server later by running the following command in the host machine:

```bash
xhost -local:* # or xhost -local:docker
```
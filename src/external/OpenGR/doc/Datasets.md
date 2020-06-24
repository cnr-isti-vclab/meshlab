# Datasets {#datasets}

If you want to go further, you can also download the demo datasets, including 3D models from the original paper. To do so, you can either call

    make dl-datasets

if you use Super4PCS from sources, or directly download this [file](https://www.irit.fr/~Nicolas.Mellado/dl/datasets/point-clouds/Super4PCS-dataset-demo1.zip) (MD5: `ad1e172902b41a3f17e9b4901adf3ba5`).

Then, call the registration script:

    cd assets/demo1
    ./run.sh   # call the run.bat script on windows

The registration output can be easily checked using the meshlab projects (files `*.mlp`) provided in the dataset subfolders.

> Note that this demo does not include local registration, ICP must be ran in post-process to obtain fine alignment.

#ifndef SNAPSHOTSETTING
#define SNAPSHOTSETTING

class SnapshotSetting
{
public:
	QString outdir;
	QString basename;
	int counter;
	int resolution;
  bool transparentBackground;
  bool snapAllLayers;
  bool tiledSave; // if true all the tiles are saved as separated files and not joined.
  bool addToRasters;
		
	SnapshotSetting()
	{
		outdir=".";
		basename="snapshot";
		counter=0;
		resolution=1;
    transparentBackground=true;
    snapAllLayers=false;
    tiledSave=false;
    addToRasters=false;
	};
};

#endif
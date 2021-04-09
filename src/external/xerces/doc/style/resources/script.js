rolloverImagesOn=new Array();
rolloverImagesOff=new Array();

function rolloverOn(name) {
  if(rolloverImagesOn[name]){
    document.images[name].src=rolloverImagesOn[name].src;
  }
}

function rolloverOff(name) {
  if(rolloverImagesOff[name]){
      document.images[name].src=rolloverImagesOff[name].src;
  }
}

function rolloverLoad(name,on,off) {
  rolloverImagesOn[name]=new Image();
  rolloverImagesOn[name].src=on;
  rolloverImagesOff[name]=new Image();
  rolloverImagesOff[name].src=off;
}

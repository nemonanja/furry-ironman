//Function to draw the pot. The canvas passed on should have aspect ratio of 21:16 but it will work with others too.
//The pot drawing is based on relative coordinates from upper left corner.
function drawPot(canvas) {
    var ctx = canvas.getContext("2d");
    var w = canvas.width;
    var h = canvas.height;
    var l = 2; //line thickness
	var c = 50; //corner size

	//clear the canvas
	ctx.clearRect(0, 0, w, h);
	//draw the handle
	ctx.beginPath();
	ctx.moveTo(0.63*w, 0.19*h);
	ctx.lineTo(0.84*w, 0.15*h);
	ctx.lineTo(0.91*w, 0.73*h);
	ctx.lineTo(0.88*w, 0.74*h);
	ctx.lineTo(0.78*w, 0.30*h);
	ctx.lineTo(0.63*w, 0.30*h);
	ctx.lineTo(0.63*w, 0.19*h);
	ctx.closePath();
	ctx.lineWidth = 1; //the handle is filled, therefore lineWidth 1 is the best option
	ctx.stroke();
	//fill the handle
	ctx.fillStyle = "black";
	ctx.fill();
	//draw the pot
	ctx.moveTo(0.19*w, 0.19*h);
	ctx.lineTo(0.17*w, 0.15*h);
	ctx.lineTo(0.13*w, 0.15*h);
	ctx.lineTo(0.14*w, 0.11*h);
	ctx.lineTo(0.21*w, 0.06*h);
	ctx.lineTo(0.67*w, 0.06*h);
	ctx.lineTo(0.63*w, 0.16*h);
	ctx.lineTo(0.63*w, 0.30*h);
	ctx.lineTo(0.70*w, 0.39*h);
	ctx.lineTo(0.72*w, 0.85*h);
	ctx.lineTo(0.68*w, 0.94*h);
	ctx.lineTo(0.15*w, 0.94*h);
	ctx.lineTo(0.10*w, 0.85*h);
	ctx.lineTo(0.13*w, 0.39*h);
	ctx.lineTo(0.19*w, 0.30*h);
	ctx.lineTo(0.19*w, 0.19*h);
	ctx.lineWidth = l;
	ctx.stroke();
	//Create the metal bar to the upper part of the pot.
	ctx.fillStyle = "grey";
	ctx.fillRect(0.19*w, 0.19*h, 0.44*w, 0.10*h);
	//Clip the current drawing to hide the rest of the fillRect when the pot is filled.
	ctx.clip();
}

//Function to fill the drawn pot. The pot fill value should be from 0 to 100. 
//When the pot is full (100%) it will be filled to the corner that is little lower than the metal bar.
function fillPot(canvas, percentage) {
    var ctx = canvas.getContext("2d");
    var w = canvas.width;
    var h = canvas.height;
    ctx.clearRect(0.10*w, 0.30*h, 0.63*w, h);
	ctx.fillStyle = "black";
	ctx.fillRect(0.10*w, (100-percentage)/100 *0.59*h + 0.34*h, 0.63*w, percentage/100*0.63*h);
}
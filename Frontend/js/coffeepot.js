function drawPot(canvas) {
    var ctx = canvas.getContext("2d");
    var w = canvas.width;
    var h = canvas.height;
    var l = 2; //line thickness
	var c = 50; //corner size

	//draw the handle
	ctx.moveTo(0.63*w, 0.19*h);
	ctx.lineTo(0.84*w, 0.15*h);
	ctx.lineTo(0.91*w, 0.73*h);
	ctx.lineTo(0.88*w, 0.74*h);
	ctx.lineTo(0.78*w, 0.30*h);
	ctx.lineTo(0.63*w, 0.30*h);
	ctx.lineTo(0.63*w, 0.19*h);
	ctx.lineWidth = l;
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
	//create the metal bar
	ctx.fillStyle = "grey";
	ctx.fillRect(0.19*w, 0.19*h, 0.44*w, 0.10*h);
	ctx.clip();
	//fill the pot with coffee :)
	ctx.fillStyle = "black";
	ctx.fillRect(0, 0.35*h, w, 0.59*h);
}

function fillPot(canvas, percentage) {
    var ctx = canvas.getContext("2d");
    var w = canvas.width;
    var h = canvas.height;
    ctx.clearRect(0.10*w, 0.30*h, 0.63*w, h);
	ctx.fillStyle = "black";
	ctx.fillRect(0.10*w, (100-percentage)/100 *0.65*h + 0.35*h, 0.63*w, percentage/100*0.60*h);
}

function changeSize() {
    var context = canvas.getContext('2d');
    fillPot(canvas, document.getElementById('inputSize').value);
}
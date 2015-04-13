var plot;
var plotDataPoints = 30;
var plotDataAmount = [];
var plotDataTemp = [];
var interval = 2;
var canvas;

$(document).ready(function(){       	
	setView();
	loadStatus();

	setInterval(function(){
		loadStatus();
	}, interval*1000);

	$(window).resize(function() {
		setView();
	});

	$("#btn").click(
		function(){
        	$("#btn").text(new Date($.now()));

    	}
    );
});

function loadStatus() {
	$.get("http://ufn.virtues.fi/~virtual_currency/v2/matti.php/test/", function(data) {
		$(".amount").html(data.amount + " %");
		$(".temp").html(data.temperature + " &deg;C");
		fillPot(document.getElementById("pot"), data.amount);
		updatePlotData(data);
		createPlot();
	}, "json");	
}

function createPlot() {
	if (plot) {
    	plot.destroy();
    }
	plot = $.jqplot("chart",  [plotDataAmount, plotDataTemp], {
		axes: {
			title: 'amount of coffee left',
			yaxis: {
				min: 0,
				forceTickAt0: true,
				max: 100,
				forceTickAt100: true,
				numberTicks: 6
			},
			xaxis: {
				renderer:$.jqplot.DateAxisRenderer,
				numberTicks: 20
			}
		}
	});
}

function updatePlotData(data) {
	if(typeof plotDataAmount != 'undefined' && plotDataAmount.length == plotDataPoints) {
    	plotDataAmount.shift();
	}
	plotDataAmount.push([new Date(data.timestamp_unix*1000), data.amount]);
	
	if(typeof plotDataTemp != 'undefined' && plotDataTemp.length == plotDataPoints) {
    	plotDataTemp.shift();
	}
	plotDataTemp.push([new Date(data.timestamp_unix*1000), data.temperature]);
}

function setView() {
	$(".status").html(window.innerWidth+"x"+window.innerHeight);
	if(window.innerWidth>window.innerHeight) {
  		$("#normalview").css("display", "table");
  		$("#altview").css("display", "none");
  	} else {
  		$("#normalview").css("display", "none");
  		$("#altview").css("display", "table");
  	}
  	
  	$(".pot").each(function() {
  		if($(this).is(":visible")) {
  			$(this).attr('id', 'pot');
  		} else {
  			$(this).removeAttr('id');
  		}
  	});
  	
  	$(".chart").each(function() {
  		if($(this).is(":visible")) {
  			$(this).attr('id', 'chart');
  		} else {
  			$(this).removeAttr('id');
  		}
  	});
  	
    canvas = document.getElementById("pot");
  	$("#pot").height($("#pot").width() * 0.76);
  	drawPot(canvas);
  	
  	if(typeof plotDataAmount != 'undefined' && plotDataAmount.length > 0) {
  		createPlot();
  	}
}
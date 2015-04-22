//Load Google Charts 
google.load('visualization', '1.1', {packages: ['line']});

var chart;
var plotDataPoints = 25;
var plotData;
var options;
var interval = 2;
var redrawPlotInterval = 5;
var redrawPlotCount = 0;
var canvas;
var w;
var h;

//debugging
var i = 1;
//http://vm0103.virtues.fi/
//http://ufn.virtues.fi/~virtual_currency/v2/matti.php/test/

$(document).ready(function(){
	plotData = new google.visualization.DataTable();
	plotData.addColumn('timeofday', 'Time');
	plotData.addColumn('number', 'Amount');
	plotData.addColumn('number', 'Temperature');
	options = {
		curveType: 'function',
		chart: {
			title: 'History data'
		}
  	};
  	options['width'] = 0.9*window.innerWidth;
  	
	setInterval(function(){
		loadStatus();
	}, interval*1000);

	$(window).resize(function() {
  		options['width'] = 0.9*window.innerWidth;
		setView();
	});

	setView();
	loadStatus();
});

function loadStatus() {
	$.get("http://ufn.virtues.fi/~virtual_currency/v2/matti.php/test/", function(data) {
		$(".amount").html(data.amount + " %");
		$(".temp").html(data.temperature + " &deg;C");
		$(".status").html(data.status);
		fillPot(document.getElementById("pot"), data.amount);
		updatePlotData(data);
		redrawPlotCount = (redrawPlotCount += 1) % redrawPlotInterval;
		if(redrawPlotCount == 0) {
			createPlot();
		}
	}, "json");	
}

function createPlot() {
	chart = new google.charts.Line(document.getElementById('chart'));
    chart.draw(plotData, options);	
}


function updatePlotData(data) {
	if(plotData.getNumberOfRows() == plotDataPoints) {
		plotData.removeRow(0);
	}
	var d = new Date(data.timestamp_unix*1000);
	var time = [d.getHours(), d.getMinutes(), d.getSeconds()];
	plotData.addRows([[time, parseInt(data.amount), parseInt(data.temperature)]]);

	//console.log(plotData.getValue(0, 0));
	//debugging
	//i += 1;
	//console.log(new Date(data.timestamp_unix*1000));
	//console.log(google.visualization.dataTableToCsv(plotData));
}

function setView() {
	//$(".status").html(window.innerWidth+"x"+window.innerHeight);
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
  		
	createPlot();
}
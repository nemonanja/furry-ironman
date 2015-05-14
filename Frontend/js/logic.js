//Load Google Charts 
google.load('visualization', '1.1', {packages: ['line']});

//API endpoint
var endpoint = "http://vm0103.virtues.fi/";

//Plotting options
var interval = 3; //interval in seconds between each data fetch
var plotDataPoints = 100; //max number of datapoints in plot
var redrawPlotInterval = 5; //how many data fetches are needed before the plot is redrawn

//Other variables
var chart;
var plotData;
var options;
var redrawPlotCount = 0;
var canvas;

$(document).ready(function(){
	//Google Charts initialization
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
  	
  	//Set data fetching
	setInterval(function(){
		loadStatus();
	}, interval*1000);

	//Reload some view elements if window size changes
	$(window).resize(function() {
  		options['width'] = 0.9*window.innerWidth;
		setView();
	});

	//Set view and fetch current status
	setView();

  	//Get data history and plot if data exists
	loadHistory();
	if (plotData.getNumberOfRows() > 0) {
		createPlot();
	}
});

function loadHistory() {
	//AJAX query to fetch previous data to initialize plot
	$.get(endpoint+"history", function(data) {
		
		//Remove oldest data points if there are more data than max data points
		while(data.length > plotDataPoints) {
			data.pop();
		}

		//Add data points from oldest to latest
		while(data.length > 0) {
			updatePlotData(data.pop());
		}
	}, "json");		
}

function loadStatus() {
	//AJAX query to fetch status
	$.get(endpoint, function(data) {
		
		//Set values to correct fields
		$(".amount").html(data.amount + " %");
		$(".temp").html(data.temperature + " &deg;C");
		$(".status").html(data.status);
		
		//Fill the pot
		fillPot(document.getElementById("pot"), data.amount);
		updatePlotData(data);
		
		//Count if enough data fetches are done for plot to be redrawn
		//and redraw if true
		redrawPlotCount = (redrawPlotCount += 1) % redrawPlotInterval;
		if(redrawPlotCount == 0) {
			createPlot();
		}
	}, "json");	
}

function createPlot() {
	//Reinitialize chart variable each time to ensure functionality when screen size is changed
	chart = new google.charts.Line(document.getElementById('chart'));
    chart.draw(plotData, options);
}

function updatePlotData(data) {
	//If max number of data points stored, drop the oldest
	if(plotData.getNumberOfRows() == plotDataPoints) {
		plotData.removeRow(0);
	}
	//Change the timestamp to timeofday format and store the data
	var d = new Date(data.timestamp*1000);
	var time = [d.getHours(), d.getMinutes(), d.getSeconds()];
	plotData.addRows([[time, parseInt(data.amount), parseInt(data.temperature)]]);
}

function setView() {
	//If screen width is more than height set normal view
	if(window.innerWidth>window.innerHeight) {
  		$("#normalview").css("display", "table");
  		$("#altview").css("display", "none");
	//...or set alternative view
  	} else {
  		$("#normalview").css("display", "none");
  		$("#altview").css("display", "table");
  	}
  	
  	//Set id 'pot' to corresponding class 'pot' element that is visible and hide the other
  	$(".pot").each(function() {
  		if($(this).is(":visible")) {
  			$(this).attr('id', 'pot');
  		} else {
  			$(this).removeAttr('id');
  		}
  	});
  	
  	//Set id 'chart' to corresponding class 'chart' element that is visible and hide the other
  	$(".chart").each(function() {
  		if($(this).is(":visible")) {
  			$(this).attr('id', 'chart');
  		} else {
  			$(this).removeAttr('id');
  		}
  	});
  	
	//Select visible canvas, set correct aspect ratio for the coffee pot
	//and draw the pot
	canvas = document.getElementById("pot");
  	$("#pot").height($("#pot").width() * 0.76);
  	drawPot(canvas);
  	
  	//Create the plot
	createPlot();
}
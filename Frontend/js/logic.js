$(document).ready(function(){
	var interval = 30;
	var canvas = document.getElementById("pot");
    drawPot(canvas);
    $.jqplot("chart",  [[[1, 2],[3,5.12],[5,13.1],[7,33.6],[9,85.9],[11,219.9]]]);


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
		//$(".chart").html("Updated on " + new Date($.now()));
	}, "json");	
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

  	$("#pot").height($("#pot").width() * 0.76);
}
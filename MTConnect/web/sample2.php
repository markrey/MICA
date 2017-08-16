
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"><HTML><HEAD><title>Chart.js TEST</title></HEAD>
<body>
<div style="width: 100%"><canvas id="chart" height="400" width="400"></canvas></div>
<script src="Chart.min.js"></script><script>
var lineChartData = {
labels : [
"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"
],
datasets : [{type: 'bar',label: "trend01",strokeColor : "rgba(255,0,0,1.0)",pointColor : "rgba(255,0,0,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(255,0,0,1.0)",

data : [0.987292,0.293255,0.391007,0.488759,0.586510,0.684262,0.782014,0.879765,0.977517,0.879765,0.782014,0.684262,0.586510,0.488759,0.391007,0.195503]
},{label: "PreHigh",borderWidth:1,backgroundColor: "#ffa000",borderColor: "#ffa000",
data : [0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000]
},{label: "HighAlm",borderWidth:1,backgroundColor: "#ff0000",borderColor: "#ff0000",
data : [0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000]

}
]
}
var options = {
  showTooltips : false, 
  scaleOverlay : true, 
  scaleOverride : false, 
  scaleSteps : 10, 
  scaleStepWidth : 10, 
  scaleStartValue : 0, 
  scaleLineColor : "rgba(0, 0, 0, 0.1)", scaleLineWidth : 1, scaleShowLabels : true, scaleLabel : "  <%=value%>g/L", scaleFontFamily : "'Arial'", scaleFontSize : 12, scaleFontStyle : "normal", scaleFontColor : "#666", scaleShowGridLines : true, scaleGridLineColor : "rgba(0, 0, 0, 0.05)", 
  scaleGridLineWidth : 1, 
  bezierCurve : false, 
  pointDot : false, 
  pointDotRadius : 5, 
  pointDotStrokeWidth : 1, 
  datasetStroke : false, 
  datasetStrokeWidth : 2, 
  datasetFill : false, 
  animation : false, animationSteps : 60, animationEasing : "easeOutQuad", 
  stacked: true, //êœÇ›è„Ç∞ñ_ÉOÉâÉtÇ…Ç∑ÇÈê›íË
  onAnimationComplete : null }
window.onload = function(){ var ctx1 = document.getElementById("chart").getContext("2d"); window.myLine = new Chart(ctx1).Bar(lineChartData, options);}
</script></body></html>

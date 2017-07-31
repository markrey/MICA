<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
<HEAD>
<title>Chart.js TEST</title>
</HEAD>

<body>
  <div style="width: 100%">
    <canvas id="chart" height="300" width="400"></canvas>
  </div>

  <script src="Chart.min.js"></script>
  <!-- もしくは<script src="Chart.min.js"></script> -->
  <script>
  var barChartData = {
    labels : ["A1","A2","A3","A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","A16"],
    datasets : [
      {
        type: 'line',
        label: "私",
        fillColor : /*"#f2dae8"*/"rgba(242,218,232,0.6)",
        strokeColor : /*"#dd9cb4"*/"rgba(221,156,180,0.6)",
        pointColor : /*"#dd9cb4"*/"rgba(221,156,180,0.6)",
        pointStrokeColor : "#fff",
        pointHighlightFill : "#fff",
        pointHighlightStroke : /*"#dd9cb4"*/"rgba(221,156,180,0.6)",
data : [31.000000,32.000000,33.000000,34.000000,35.000000,36.000000,37.000000,38.000000,39.000000,40.000000,41.000000,42.000000,43.000000,44.000000,45.000000,46.000000]
      }
    ]

  }



  window.onload = function(){
    var ctx2 = document.getElementById("chart").getContext("2d");
    window.myBar = new Chart(ctx2).Bar(barChartData, {responsive: true, animation : false, bezierCurve : false});
  }  
</script>

<?php
?>

</body>

</html>


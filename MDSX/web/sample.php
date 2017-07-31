
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
<HEAD>
<title>Chart.js TEST</title>
</HEAD>

<body>
  <div style="width: 100%">
    <canvas id="chart" height="300" width="400"></canvas>
  </div>

<script src="Chart.js"></script>

<script>
  var ctx = document.getElementById("chart");
  var chart = new Chart(ctx, {
      type: 'bar',
      data: {
          labels : ["1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"],
datasets: [{label: "Normal",borderWidth:1,backgroundColor: "#00ff7f",borderColor: "#00ff7f",
   pointHighlightFill: "rgba(255,255,255,0.0)",
   pointHighlightStroke: "rgba(255,255,255,0.0)",
data : [0.987292,0.293255,0.391007,0.488759,0.586510,0.684262,0.782014,0.879765,0.977517,0.879765,0.782014,0.684262,0.586510,0.488759,0.391007,0.195503]
},{label: "PreHigh",borderWidth:1,backgroundColor: "#ffa000",borderColor: "#ffa000",
data : [0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000]
},{label: "HighAlm",borderWidth:1,backgroundColor: "#ff0000",borderColor: "#ff0000",
data : [0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000]
}]
      },
      options: {
          scales: {
              xAxes: [{
                    stacked: true, //積み上げ棒グラフにする設定
                    categoryPercentage:0.4 //棒グラフの太さ
              }],
              yAxes: [{
                    stacked: true //積み上げ棒グラフにする設定
                    
              }]
          }
         ,legend: {
            display: false
         }
       ,responsive: true, animation : false, bezierCurve : false      
       
       }
  });
  chart.options.tooltips.enabled = false;
</script>



</body>

</html>

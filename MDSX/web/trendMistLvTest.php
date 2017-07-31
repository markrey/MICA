<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"><HTML><HEAD><title>Chart.js TEST</title></HEAD><body><div style="width: 100%"><canvas id="chart" height="400" width="400"></canvas></div><script src="Chart.min.js"></script><script>
var lineChartData = {
labels : ["16:56:20","","","","","16:56:26","","","","","16:56:33","","","","","16:56:40","","","","","16:56:47","","","","","16:56:54","","","","","16:57:00","","","","","16:57:07","","","","","16:57:14","","","","","16:57:21","","","","","16:57:28","","","","","16:57:35","","","","","16:57:41","","","","","16:57:48","","","","","16:57:55","","","","","16:58:02","","","","","16:58:09","","","","","16:58:16","","","","","16:58:22","","","","","16:58:29","","","","","16:58:36","","","","","16:58:43","","","","","16:58:50","","","","","16:58:57","","","","16:59:02",],datasets : [{
type: 'line',label: "trend01",strokeColor : "rgba(255,0,0,1.0)",pointColor : "rgba(255,0,0,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(255,0,0,1.0)",
data : [1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809,1.964809]
},{
type: 'line',label: "trend02",strokeColor : "rgba(0,0,255,1.0)",pointColor : "rgba(0,0,255,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(0,0,255,1.0)",
data : [0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255,0.293255]
},{
type: 'line',label: "trend03",strokeColor : "rgba(0,255,0,1.0)",pointColor : "rgba(0,255,0,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(0,255,0,1.0)",
data : [0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007]
},{
type: 'line',label: "trend04",strokeColor : "rgba(255,255,0,1.0)",pointColor : "rgba(255,255,0,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(255,255,0,1.0)",
data : [0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759]
},{
type: 'line',label: "trend05",strokeColor : "rgba(0,255,255,1.0)",pointColor : "rgba(0,255,255,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(0,255,255,1.0)",
data : [0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510]
},{
type: 'line',label: "trend06",strokeColor : "rgba(255,0,255,1.0)",pointColor : "rgba(255,0,255,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(255,0,255,1.0)",
data : [0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262]
},{
type: 'line',label: "trend07",strokeColor : "rgba(247,147,30,1.0)",pointColor : "rgba(247,147,30,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(247,147,30,1.0)",
data : [0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014]
},{
type: 'line',label: "trend08",strokeColor : "rgba(115,75,35,1.0)",pointColor : "rgba(115,75,35,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(115,75,35,1.0)",
data : [0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765]
},{
type: 'line',label: "trend09",strokeColor : "rgba(194,153,107,1.0)",pointColor : "rgba(194,153,107,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(194,153,107,1.0)",
data : [0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517,0.977517]
},{
type: 'line',label: "trend10",strokeColor : "rgba(17,161,150,1.0)",pointColor : "rgba(17,161,150,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(17,161,150,1.0)",
data : [0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765,0.879765]
},{
type: 'line',label: "trend11",strokeColor : "rgba(220,38,83,1.0)",pointColor : "rgba(220,38,83,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(220,38,83,1.0)",
data : [0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014,0.782014]
},{
type: 'line',label: "trend12",strokeColor : "rgba(29,43,83,1.0)",pointColor : "rgba(29,43,83,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(29,43,83,1.0)",
data : [0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262,0.684262]
},{
type: 'line',label: "trend13",strokeColor : "rgba(149,28,90,1.0)",pointColor : "rgba(149,28,90,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(149,28,90,1.0)",
data : [0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510,0.586510]
},{
type: 'line',label: "trend14",strokeColor : "rgba(208,215,47,1.0)",pointColor : "rgba(208,215,47,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(208,215,47,1.0)",
data : [0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759,0.488759]
},{
type: 'line',label: "trend15",strokeColor : "rgba(0,101,54,1.0)",pointColor : "rgba(0,101,54,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(0,101,54,1.0)",
data : [0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007,0.391007]
},{
type: 'line',label: "trend16",strokeColor : "rgba(193,39,45,1.0)",pointColor : "rgba(193,39,45,1.0)",pointStrokeColor : "#fff",pointHighlightFill : "#fff",pointHighlightStroke : "rgba(193,39,45,1.0)",
data : [0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503,0.195503]
}]
}
var options = {
	scaleOverlay : true, 
	scaleOverride : false,
	scaleSteps : 10, 
	scaleStepWidth : 10, 
	scaleStartValue : 0, 
	scaleLineColor : "rgba(0, 0, 0, 0.1)", 
	scaleLineWidth : 1, 
	scaleShowLabels : true, 
	scaleLabel : "  <%=value%>mg/L", 
	scaleFontFamily : "'Arial'", 
	scaleFontSize : 12, 
	scaleFontStyle : "normal", 
	scaleFontColor : "#666", 
	scaleShowGridLines : true, 
	scaleGridLineColor : "rgba(0, 0, 0, 0.05)", 
	scaleGridLineWidth : 1, 
	bezierCurve : false, 
	pointDot : false, 
	pointDotRadius : 5, 
	pointDotStrokeWidth : 1, 
	datasetStroke : false, 
	datasetStrokeWidth : 2, 
	datasetFill : false, 
	animation : false, 
	animationSteps : 60, 
	animationEasing : "easeOutQuad", 
	onAnimationComplete : null }
window.onload = function(){
	var ctx1 = document.getElementById("chart").getContext("2d"); window.myLine = new Chart(ctx1).Line(lineChartData, options);
}
</script><?php ?></body></html>
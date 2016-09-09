<html>
<head>
<link rel="stylesheet" href="../resource/combine/combine.css">
<script src="../resource/combine/combine.js"></script>
<!--
<script src="http://code.jquery.com/jquery-1.8.2.js"></script>
<script src="http://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['controls', 'charteditor']}]}"></script>
<script src="http://jquery-csv.googlecode.com/git/src/jquery.csv.js"></script>
<link rel="stylesheet" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/css/bootstrap.min.css">
<link rel="stylesheet" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/css/bootstrap-theme.min.css">
<script src="http://maxcdn.bootstrapcdn.com/bootstrap/3.2.0/js/bootstrap.min.js"></script>
-->
<script>
$(function() {
  google.load("visualization", "1", {packages:["corechart"]});

  var csv = $('#textInput').val();
  var dt = google.visualization.arrayToDataTable($.csv.toArrays(csv, {onParseValue: $.csv.hooks.castToScalar}));

  function sortCaseInsensitive(dt, column) {
    for (var row = 0; row < dt.getNumberOfRows(); row++) {
      var s = dt.getValue(row, column);
      dt.setValue(row, column, s.toUpperCase());
      dt.setFormattedValue(row, column, s);
    }
    dt.sort(column);
  }

  function AddToOverall(dt) {
    if (overallDt == null) {
      overallDt = dt.clone();
    }
    else {
      var col1 = [];
      for (var i = 1; i < overallDt.getNumberOfColumns(); i++)
        col1.push(i);
      overallDt = google.visualization.data.join(overallDt, dt, 'full', [[0, 0]], col1, [1]);
    }

    var newCol = overallDt.getNumberOfColumns() - 1;
    overallDt.setColumnLabel(newCol, type + ' ' + overallDt.getColumnLabel(newCol))
    var formatter1 = new google.visualization.NumberFormat({ fractionDigits: 0 });
    formatter1.format(overallDt, newCol);

    for (var row = 0; row < overallDt.getNumberOfRows(); row++) {
      if (overallDt.getValue(row, newCol) == null) {
        overallDt.setValue(row, newCol, Number.POSITIVE_INFINITY);
        overallDt.setFormattedValue(row, newCol, "");
      }
    }
  }

  function createSortEvent(type, dt, chart) {
    return function(e) {
      if (e.column == 0 || e.column == 1) {
        var t = dt.clone();
        drawBarChart(type, t, chart, [{column: e.column, desc: !e.ascending }]);
      }
    }
  }

  addSection("0. Overall");

  var overallDiv = document.createElement("div");
  overallDiv.className = "tablechart";
  $("#main").append(overallDiv);

  // Per type sections
  var types = dt.getDistinctValues(0);
  var overallDt;
  for (var i in types) {
    var type = types[i];
    addSection(type);

    var view = new google.visualization.DataView(dt);
    view.setRows(view.getFilteredRows([{column: 0, value: type}]));

    if (type.search("Code size") != -1) {
      var sizedt = google.visualization.data.group(
        view,
        [1], 
        [{"column": 7, "aggregation": google.visualization.data.sum, 'type': 'number' }]
      );
      AddToOverall(sizedt);
      sortCaseInsensitive(sizedt, 0);
      addSubsection(sizedt.getColumnLabel(1));
      var sizeTable = drawTable(type, sizedt.clone(), false);
      var sizeChart = drawBarChart(type, sizedt.clone());
      google.visualization.events.addListener(sizeTable, 'sort', createSortEvent(type, sizedt, sizeChart));
    }
    else {
      addSubsection("Time");

      var timedt = google.visualization.data.group(
        view,
        [1], 
        [{"column": 3, "aggregation": google.visualization.data.sum, 'type': 'number' }]
      );

      AddToOverall(timedt);
      sortCaseInsensitive(timedt, 0);
      var timeTable = drawTable(type, timedt.clone(), true);
      var timeChart = drawBarChart(type, timedt.clone());
      google.visualization.events.addListener(timeTable, 'sort', createSortEvent(type, timedt, timeChart));

      // Per JSON
      drawPivotBarChart(
        type + " per JSON",
        pivotTable(google.visualization.data.group(
          view,
          [2, 1],
          [{"column": 3, "aggregation": google.visualization.data.sum, 'type': 'number' }]
        )),
        dt.getColumnLabel(3)
      );

      // Only show memory of Parse
      if (type.search("Parse") != -1) {
        for (var column = 4; column <= 6; column++) {
          var memorydt = google.visualization.data.group(
            view,
            [1], 
            [{"column": column, "aggregation": google.visualization.data.sum, 'type': 'number' }]
          );
          AddToOverall(memorydt);
          sortCaseInsensitive(memorydt, 0);
          addSubsection(memorydt.getColumnLabel(1));
          var memoryTable = drawTable(type, memorydt.clone(), false);
          var memoryChart = drawBarChart(type, memorydt.clone());

          google.visualization.events.addListener(memoryTable, 'sort', createSortEvent(type, memorydt, memoryChart));
        }
      }
    }
  }

  var overallTable = new google.visualization.Table(overallDiv);
  sortCaseInsensitive(overallDt, 0);
  overallTable.draw(overallDt);

  $(".chart").each(function() {
    var chart = $(this);
    var d = $("#downloadDD").clone().css("display", "");
    $('li a', d).each(function() {
          $(this).click(function() {
              var svg = chart[0].getElementsByTagName('svg')[0].parentNode.innerHTML;
              svg=sanitize(svg);
              $('#imageFilename').val($("#title").html() + "_" + chart.data("filename"));
              $('#imageGetFormTYPE').val($(this).attr('dltype'));
              $('#imageGetFormSVG').val(svg);
              $('#imageGetForm').submit();
          });
      });   
    $(this).after(d);
  });

  // Add configurations
  var thisConfig = <?="\"".basename($argv[1], '.'.pathinfo($argv[1], PATHINFO_EXTENSION))."\""?>;
  var configurations = [<?=
    implode(",", 
      array_map(
        function ($filename) {
          return "\"" . basename($filename, ".csv") . "\"";
        }, glob("*.csv")))
    ?>];

  for (var i in configurations) {
    var c = configurations[i];
    $("#benchmark").append($("<li>", {class : (c == thisConfig ? "active" : "")}).append($("<a>", {href: c + ".html"}).append(c)));
  }
});

function pivotTable(src) {
  var dst = new google.visualization.DataTable();
  
  // Add columns
  var key = src.getDistinctValues(1);
  var keyColumnMap = {};
  dst.addColumn(src.getColumnType(0), src.getColumnLabel(0));  
  for (var k in key)
    keyColumnMap[key[k]] = dst.addColumn(src.getColumnType(2), key[k]);

  // Add rows
  var pivot = src.getDistinctValues(0);
  var pivotRowMap = {};  
  for (var p in pivot)
    dst.setValue(pivotRowMap[[pivot[p]]] = dst.addRow(), 0, pivot[p]);

  // Fill cells
  for (var row = 0; row < src.getNumberOfRows(); row++)
    dst.setValue(
      pivotRowMap[src.getValue(row, 0)],
      keyColumnMap[src.getValue(row, 1)],
      src.getValue(row, 2));

  return dst;
}

function addSection(name) {
  $("#main").append(
    $("<a>", {"name": name}),
    $("<h2>", {style: "padding-top: 70px; margin-top: -70px;"}).append(name)
  );
  $("#section").append($("<li>").append($("<a>", {href: "#" + name}).append(name)));
}

function addSubsection(name) {
  $("#main").append(
    $("<h3>", {style: "padding-top: 70px; margin-top: -70px;"}).append(name)
  );
}

function drawTable(type, data, isSpeedup) {
  if (isSpeedup)
    data.addColumn('number', 'Speedup');
  else
    data.addColumn('number', 'Ratio');
  //data.sort([{ column: 1, desc: true }]);
  var formatter1 = new google.visualization.NumberFormat({ fractionDigits: 0 });
  formatter1.format(data, 1);

  var div = document.createElement("div");
  div.className = "tablechart";
  $("#main").append(div);
    var table = new google.visualization.Table(div);
    redrawTable(0);
    table.setSelection([{ row: 0, column: null}]);

  function redrawTable(selectedRow) {
      var s = table.getSortInfo();
      // Compute relative time using the first row as basis
      var basis = data.getValue(selectedRow, 1);
      for (var rowIndex = 0; rowIndex < data.getNumberOfRows(); rowIndex++)
        data.setValue(rowIndex, 2, isSpeedup ? basis / data.getValue(rowIndex, 1) : data.getValue(rowIndex, 1) / basis);

      var formatter = new google.visualization.NumberFormat({suffix: 'x'});
      formatter.format(data, 2); // Apply formatter to second column

      table.draw(data, s != null ? {sortColumn: s.column, sortAscending: s.ascending} : null);
  }

  google.visualization.events.addListener(table, 'select',
  function() {
      var selection = table.getSelection();
      if (selection.length > 0) {
          var item = selection[0];
          if (item.row != null)
              redrawTable(item.row);
      }
  });

  return table;
}

function drawBarChart(type, data, chart, sortOptions) {
  // Using same colors as in series
  var colors = ["#3366cc","#dc3912","#ff9900","#109618","#990099","#0099c6","#dd4477","#66aa00","#b82e2e","#316395","#994499","#22aa99","#aaaa11","#6633cc","#e67300","#8b0707","#651067","#329262","#5574a6","#3b3eac","#b77322","#16d620","#b91383","#f4359e","#9c5935","#a9c413","#2a778d","#668d1c","#bea413","#0c5922","#743411","#3366cc","#dc3912","#ff9900","#109618","#990099","#0099c6","#dd4477","#66aa00","#b82e2e","#316395","#994499","#22aa99","#aaaa11","#6633cc","#e67300","#8b0707","#651067","#329262","#5574a6","#3b3eac","#b77322","#16d620","#b91383","#f4359e","#9c5935","#a9c413","#2a778d","#668d1c","#bea413","#0c5922","#743411"];
  var h = data.getNumberOfRows() * 12;
  var options = { 
    title: type,
    chartArea: {left: '20%', width: '70%', height: h },
    width: 800,
    height: h + 100,
    fontSize: 10,
    bar: {groupWidth: "80%"},
    hAxis: { title: data.getColumnLabel(1) },
    legend: { position: "none" },
  };

  data.addColumn({ type: "string", role: "style" });
  data.addColumn({ type: "number", role: "annotation" });
  for (var rowIndex = 0; rowIndex < data.getNumberOfRows(); rowIndex++) {
    data.setValue(rowIndex, 2, colors[rowIndex]);
    data.setValue(rowIndex, 3, data.getValue(rowIndex, 1));
  }

  if (sortOptions != null)
    data.sort(sortOptions); // sort after assigning colors

  var formatter1 = new google.visualization.NumberFormat({ fractionDigits: 0 });
  formatter1.format(data, 3);

  if (chart == null) {
    var div = document.createElement("div");
    div.className = "chart";
    $(div).data("filename", type + "_" + data.getColumnLabel(1));
    $("#main").append(div);
    chart = new google.visualization.BarChart(div);
  }

  chart.draw(data, options);
  return chart;
}


function drawPivotBarChart(type, data, title) {
  var h = (data.getNumberOfColumns() + 1) * data.getNumberOfRows() * 5;
  var options = { 
    title: type,
    chartArea: {left: '10%', width: '70%', 'height': h},
    width: 800,
    height: h + 100,
    fontSize: 10,
    hAxis: { "title": title },
    legend: { textStyle: {fontSize: 8}},
    bar : { groupWidth: "95%" }
  };
  var div = document.createElement("div");
  div.className = "chart";
  $(div).data("filename", type + "_" + title);
  $("#main").append(div);
  var chart = new google.visualization.BarChart(div);

  chart.draw(data, options);
}

// http://jsfiddle.net/P6XXM/
function sanitize(svg) {
    svg = svg
        .replace(/\<svg/,'<svg xmlns="http://www.w3.org/2000/svg" version="1.1"')
        .replace(/zIndex="[^"]+"/g, '')
        .replace(/isShadow="[^"]+"/g, '')
        .replace(/symbolName="[^"]+"/g, '')
        .replace(/jQuery[0-9]+="[^"]+"/g, '')
        .replace(/isTracker="[^"]+"/g, '')
        .replace(/url\([^#]+#/g, 'url(#')
        .replace('<svg xmlns:xlink="http://www.w3.org/1999/xlink" ', '<svg ')
        .replace(/ href=/g, ' xlink:href=')
    /*.replace(/preserveAspectRatio="none">/g, 'preserveAspectRatio="none"/>')*/
    /* This fails in IE < 8
    .replace(/([0-9]+)\.([0-9]+)/g, function(s1, s2, s3) { // round off to save weight
    return s2 +'.'+ s3[0];
    })*/

    // IE specific
        .replace(/id=([^" >]+)/g, 'id="$1"')
        .replace(/class=([^" ]+)/g, 'class="$1"')
        .replace(/ transform /g, ' ')
        .replace(/:(path|rect)/g, '$1')
        .replace(/<img ([^>]*)>/gi, '<image $1 />')
        .replace(/<\/image>/g, '') // remove closing tags for images as they'll never have any content
        .replace(/<image ([^>]*)([^\/])>/gi, '<image $1$2 />') // closes image tags for firefox
        .replace(/width=(\d+)/g, 'width="$1"')
        .replace(/height=(\d+)/g, 'height="$1"')
        .replace(/hc-svg-href="/g, 'xlink:href="')
        .replace(/style="([^"]+)"/g, function (s) {
            return s.toLowerCase();
        });

    // IE9 beta bugs with innerHTML. Test again with final IE9.
    svg = svg.replace(/(url\(#highcharts-[0-9]+)&quot;/g, '$1')
        .replace(/&quot;/g, "'");
    if (svg.match(/ xmlns="/g).length == 2) {
        svg = svg.replace(/xmlns="[^"]+"/, '');
    }

    return svg;
}
</script>
<style type="text/css">
@media (min-width: 800px) {
/*  .container {
    max-width: 800px;
  }*/
}
textarea {
  font-family: Consolas, 'Liberation Mono', Menlo, Courier, monospace;
}
.tablechart {
/*  width: 500px;
*/  margin: auto;
  padding-top: 20px;
  padding-bottom: 20px;
}
.chart {
  padding-top: 20px;
  padding-bottom: 20px;
}
body { padding-top: 70px; }
</style>
</head>
<body>
<div class="container">
<nav class="navbar navbar-default navbar-fixed-top" role="navigation">
  <div class="container">
    <!-- Brand and toggle get grouped for better mobile display -->
    <div class="navbar-header">
      <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <a class="navbar-brand" href="https://github.com/miloyip/nativejson-benchmark"><span class="glyphicon glyphicon-home"></span> nativejson-benchmark</a>
    </div>

    <!-- Collect the nav links, forms, and other content for toggling -->
    <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
      <ul class="nav navbar-nav">
        <li class="dropdown">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown">Benchmark <span class="caret"></span></a>
          <ul class="dropdown-menu" role="menu" id="benchmark">
          </ul>
        </li>
        <li class="dropdown">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown">Section <span class="caret"></span></a>
          <ul class="dropdown-menu" role="menu" id="section">
          </ul>
        </li>
      </ul>
      <p class="navbar-text navbar-right">Developed by <a href="https://github.com/miloyip" class="navbar-link">Milo Yip</a></p>
    </div><!-- /.navbar-collapse -->
  </div><!-- /.container-fluid -->
</nav>
<div class="page-header">
<h1 id="title"><?=basename($argv[1], '.'.pathinfo($argv[1], PATHINFO_EXTENSION))?></h1>
</div>
<div id="main"></div>
<h2>Source CSV</h2>
<textarea id="textInput" class="form-control" rows="5" readonly>
<?php include $argv[1] ?>
</textarea>
</div>
<div class="row" id="downloadDD" style="display: none">
<div class="btn-group pull-right" >
    <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="glyphicon glyphicon-picture"></span></button>
    <ul class="dropdown-menu">
        <li><a tabindex="-1" href="#" dltype="image/jpeg">JPEG</a></li>
        <li><a tabindex="-1" href="#" dltype="image/png">PNG</a></li>
        <li><a tabindex="-1" href="#" dltype="application/pdf">PDF</a></li>
        <li><a tabindex="-1" href="#" dltype="image/svg+xml">SVG</a></li>
    </ul>
</div>
</div>
<form method="post" action="http://export.highcharts.com/" id="imageGetForm">
    <input type="hidden" name="filename" id="imageFilename" value="" />
    <input type="hidden" name="type" id="imageGetFormTYPE" value="" />
    <input type="hidden" name="width" value="1600" />
    <input type="hidden" name="svg" value="" id="imageGetFormSVG" />
</form>
</div>
</body>
</html>
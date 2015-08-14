
google.load('visualization', '1', {'packages':['corechart']});

Drupal.behaviors.prestataires_analytics = {

    attach: function(context, settings) {
        
        if(typeof chartData == 'undefined') {return;}
        
        drawChart();
    }
};

function drawChart() {

    if(typeof chartData == 'undefined') {return;}

    var options = {
        legend: 'none',
        series: {0: {color: chartData.color}},
        chartArea: {width: '90%', height: '80%'},
        title: chartData.title,
        titlePosition: 'in',
        titleTextStyle: {color: chartData.fontColor},
        hAxis: {
            textStyle: {fontSize: 12, color: chartData.fontColor}},
        vAxis: {
            textStyle: {fontSize: 12, color: chartData.fontColor},
            viewWindow: {min: 0},
        },
        tooltip: {textStyle: {color: chartData.fontColor}}
    };

    var data = new google.visualization.DataTable();

    switch(chartData.dimension) {

        case 'date':
            data.addColumn('string', chartData.xAxisTitle);

            options.pointSize = 3;

            break;

        case 'isoWeek':
            data.addColumn('string', chartData.xAxisTitle);

            options.bar = {groupWidth: '95%'};

            break;

        case 'month':
            data.addColumn('string', chartData.xAxisTitle);

            options.bar = {groupWidth: '95%'};

            break;
    }

    data.addColumn('number', chartData.yAxisTitle);

    data.addRows(chartData.rows);

    switch(chartData.dimension) {

        case 'date':
            var chart = new google.visualization.LineChart(document.getElementById('analytics-chart'));
            break;

        case 'isoWeek':
        case 'month':
            var chart = new google.visualization.ColumnChart(document.getElementById('analytics-chart'));
            break;
    }

    chart.draw(data, options);
}

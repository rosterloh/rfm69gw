var websock;
var tabindex = $("#mapping > div").length * 3 + 50;

function doUpdate() {
    var self = $(this);
    self.addClass("loading");
    $.ajax({
        'method': 'POST',
        'url': '/post',
        'dataType': 'json',
        'data': $("#formSave").serializeArray()
    }).done(function(data) {
        self.removeClass("loading");
    }).fail(function() {
        self.removeClass("loading");
    });
}

function showPanel() {
    $(".panel").hide();
    $("#" + $(this).attr("data")).show();
    if ($("#layout").hasClass('active')) toggleMenu();
};

function addMapping() {
    var template = $("#template .pure-g")[0];
    var line = $(template).clone();
    $(line).find("input").each(function() {
        $(this).attr("tabindex", tabindex++);
    });
    $(line).find("button").on('click', delMapping);
    line.appendTo("#mapping");
}

function delMapping() {
    var parent = $(this).parent().parent();
    $(parent).remove();
}

function toggleMenu() {
    $("#layout").toggleClass('active');
    $("#menu").toggleClass('active');
    $("#menuLink").toggleClass('active');
}

function processData(data) {

    // pre-process
    data.network = data.network.toUpperCase();
    data.mqttStatus = data.mqttStatus ? "CONNECTED" : "NOT CONNECTED";

    // title
    document.title = data.app;
    $(".pure-menu-heading").html(data.app);

    // automatic assign
    Object.keys(data).forEach(function(key) {
        var id = "input[name=" + key + "]";
        if ($(id).length) $(id).val(data[key]);
    });

    // WIFI
    var groups = $("#panel-wifi .pure-g");
    for (var i in data.wifi) {
        var wifi = data.wifi[i];
        Object.keys(wifi).forEach(function(key) {
            var id = "input[name=" + key + "]";
            if ($(id, groups[i]).length) $(id, groups[i]).val(wifi[key]);
        });
    };

    // TOPICS
    for (var i in data.mapping) {

        // add a new row
        addMapping();

        // get group
        var line = $("#mapping .pure-g")[i];

        // fill in the blanks
        var mapping = data.mapping[i];
        Object.keys(mapping).forEach(function(key) {
            var id = "input[name=" + key + "]";
            if ($(id, line).length) $(id, line).val(mapping[key]);
        });

    }

}

function init() {
    $("#menuLink").on('click', toggleMenu);
    $(".button-add").on('click', addMapping);
    $(".button-del").on('click', delMapping);
    $(".button-update").on('click', doUpdate);
    $(".pure-menu-link").on('click', showPanel);
    $.ajax({'method': 'GET', 'url': '/get', 'dataType': 'json'}).done(processData);
    websock = new WebSocket('ws://' + window.location.hostname + ':81/');
    websock.onopen = function(evt) { console.log('[WEBSOCKET] Open'); };
    websock.onclose = function(evt) { console.log('[WEBSOCKET] Close'); };
    websock.onerror = function(evt) { console.log('[WEBSOCKET] Error'); console.log(evt.data); };
    websock.onmessage = function(evt) {
        console.log('[WEBSOCKET] Message');
        console.log(evt.data);
        //var data = JSON.parse(evt.data);
    };
}

$(init);

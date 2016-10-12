var websock;
var tabindex = $("#mapping > div").length * 3 + 50;

function doUpdate() {
    var self = $(this);
    self.addClass("loading");
    $.ajax({
        'method': 'POST',
        'url': '/save',
        'dataType': 'json',
        'data': $("#formSave").serializeArray()
    }).done(function(data) {
        self.removeClass("loading");
    }).fail(function() {
        self.removeClass("loading");
    });
}

function doReset() {
    var response = window.confirm("Are you sure you want to reset the device?");
    if (response == false) return;
    var self = $(this);
    self.addClass("loading");
    $.ajax({
        'method': 'GET',
        'url': '/reset'
    });
}

function doReconnect() {
    var response = window.confirm("Are you sure you want to disconnect from the current WIFI network?");
    if (response == false) return;
    var self = $(this);
    self.addClass("loading");
    $.ajax({
        'method': 'GET',
        'url': '/reconnect'
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
    if ("network" in data) {
        data.network = data.network.toUpperCase();
    }
    if ("mqttStatus" in data) {
        data.mqttStatus = data.mqttStatus ? "CONNECTED" : "NOT CONNECTED";
    }

    // title
    if ("app" in data) {
        $(".pure-menu-heading").html(data.app);
        var title = data.app;
        if ("hostname" in data) {
            title = data.hostname + " - " + title;
        }
        document.title = title;
    }

    // automatic assign
    Object.keys(data).forEach(function(key) {

        // Look for INPUTs
        var element = $("input[name=" + key + "]");
        if (element.length > 0) {
            if (element.attr('type') == 'checkbox') {
                element.prop("checked", data[key] == 1)
                    .iphoneStyle({
                        resizeContainer: false,
                        resizeHandle: false,
                        checkedLabel: 'ON',
                        uncheckedLabel: 'OFF'
                    })
                    .iphoneStyle("refresh");
            } else {
                element.val(data[key]);
            }
        }

        // Look for SELECTs
        var element = $("select[name=" + key + "]");
        if (element.length > 0) {
            element.val(data[key]);
        }

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

function getJson(str) {
    try {
        return JSON.parse(str);
    } catch (e) {
        return false;
    }
}

function init() {

    $("#menuLink").on('click', toggleMenu);
    $(".button-add").on('click', addMapping);
    $(".button-del").on('click', delMapping);
    $(".button-update").on('click', doUpdate);
    $(".button-reset").on('click', doReset);
    $(".button-reconnect").on('click', doReconnect);
    $(".pure-menu-link").on('click', showPanel);

    var host = window.location.hostname;
    //host = "rfm69gw.local";
    websock = new WebSocket('ws://' + host + ':81/');
    websock.onopen = function(evt) {};
    websock.onclose = function(evt) {};
    websock.onerror = function(evt) {};
    websock.onmessage = function(evt) {
        var data = getJson(evt.data);
        if (data) processData(data);
    };

}

$(init);

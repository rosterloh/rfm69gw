var websock;
var tabindex = $("#mapping > div").length * 3 + 50;
var messages;
var filters = [];

function doUpdate() {
    var data = $("#formSave").serializeArray();
    websock.send(JSON.stringify({'config': data}));
    $(".powExpected").val(0);
    return false;
}

function doReset() {
    var response = window.confirm("Are you sure you want to reset the device?");
    if (response == false) return false;
    websock.send(JSON.stringify({'action': 'reset'}));
    return false;
}

function doClearCounts() {
    websock.send(JSON.stringify({'action': 'clear-counts'}));
    return false;
}

function doReconnect() {
    var response = window.confirm("Are you sure you want to disconnect from the current WIFI network?");
    if (response == false) return false;
    websock.send(JSON.stringify({'action': 'reconnect'}));
    return false;
}

function doFilter(e) {
    var index = messages.cell(this).index();
    if (index == 'undefined') return;
    var c = index.column;
    var column = messages.column(c);
    if (filters[c]) {
        filters[c] = false;
        column.search("");
        $(column.header()).removeClass("filtered");
    } else {
        filters[c] = true;
        var data = messages.row(this).data();
        if (e.which == 1) {
            column.search('^' + data[c] + '$', true, false );
        } else {
            column.search('^((?!(' + data[c] + ')).)*$', true, false );
        }
        $(column.header()).addClass("filtered");
    }
    column.draw();
    return false;
}

function doClearFilters() {
    for (var i = 0; i < messages.columns()[0].length; i++) {
        if (filters[i]) {
            filters[i] = false;
            var column = messages.column(i);
            column.search("");
            $(column.header()).removeClass("filtered");
            column.draw();
        }
    }
    return false;
}

function showPanel() {
    $(".panel").hide();
    $("#" + $(this).attr("data")).show();
    if ($("#layout").hasClass('active')) toggleMenu();
    $("input[type='checkbox']").iphoneStyle("calculateDimensions").iphoneStyle("refresh");
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

    // title
    if ("app" in data) {
        $(".pure-menu-heading").html(data.app);
        var title = data.app;
        if ("hostname" in data) {
            title = data.hostname + " - " + title;
        }
        document.title = title;
    }

    Object.keys(data).forEach(function(key) {

        // Packet
        if (key == "packet") {
            var packet = data.packet;
            var d = new Date();
            messages.row.add([
                d.toLocaleTimeString('en-US', { hour12: false }),
                packet.senderID,
                packet.packetID,
                packet.targetID,
                packet.name,
                packet.value,
                packet.rssi,
                packet.duplicates,
                packet.missing,
            ]).draw(false);
            return;
        }

        // Wifi
        if (key == "wifi") {
            var groups = $("#panel-wifi .pure-g");
            for (var i in data.wifi) {
                var wifi = data.wifi[i];
                Object.keys(wifi).forEach(function(key) {
                    var id = "input[name=" + key + "]";
                    if ($(id, groups[i]).length) $(id, groups[i]).val(wifi[key]);
                });
            };
            return;
        }

		    // Topics
    		if (key == "mapping") {
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
    			  return;
    		}

        // Messages
        if (key == "message") {
            window.alert(data.message);
            return;
        }

        // Enable options
        if (key.endsWith("Visible")) {
            var module = key.slice(0,-7);
            console.log(module);
            $(".module-" + module).show();
            return;
        }

        // Pre-process
        if (key == "network") {
            data.network = data.network.toUpperCase();
        }
        if (key == "mqttStatus") {
            data.mqttStatus = data.mqttStatus ? "CONNECTED" : "NOT CONNECTED";
        }

        // Look for INPUTs
        var element = $("input[name=" + key + "]");
        if (element.length > 0) {
            if (element.attr('type') == 'checkbox') {
              element
                    .prop("checked", data[key])
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
            return;
        }

        // Look for SELECTs
        var element = $("select[name=" + key + "]");
        if (element.length > 0) {
            element.val(data[key]);
            return;
        }

    });

}

function getJson(str) {
    try {
        return JSON.parse(str);
    } catch (e) {
        return false;
    }
}

function initWebSocket(host) {
    if (host === undefined) {
        host = window.location.hostname;
    }
    websock = new WebSocket('ws://' + host + '/ws');
    websock.onopen = function(evt) {};
    websock.onclose = function(evt) {};
    websock.onerror = function(evt) {};
    websock.onmessage = function(evt) {
        var data = getJson(evt.data);
        if (data) processData(data);
    };
}

function init() {

    $("#menuLink").on('click', toggleMenu);
    $(".button-add").on('click', addMapping);
    $(".button-del").on('click', delMapping);
    $(".button-update").on('click', doUpdate);
    $(".button-reset").on('click', doReset);
    $(".button-reconnect").on('click', doReconnect);
    $(".button-clear-counts").on('click', doClearCounts);
    $(".button-clear-filters").on('click', doClearFilters);
    $(".pure-menu-link").on('click', showPanel);
    $('#messages tbody').on('mousedown', 'td', doFilter);

    messages = $('#messages').DataTable({
        "paging": false
    });

    for (var i = 0; i < messages.columns()[0].length; i++) {
        filters[i] = false;
    }

    $("input[type='checkbox']")
        .iphoneStyle({
            resizeContainer: true,
            resizeHandle: true,
            checkedLabel: 'ON',
            uncheckedLabel: 'OFF'
        })
        .iphoneStyle("refresh");


    $.ajax({
        'method': 'GET',
        'url': '/auth'
    }).done(function(data) {
        initWebSocket();
    });

}

$(init);

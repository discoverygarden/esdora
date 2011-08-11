 
$(document).ready(function(){
	init();
	//allowPan();
	toggleControl();
        
});
	
var map, drawControls;
function init(){
	    map = new OpenLayers.Map('map');

        var wmsLayer = new OpenLayers.Layer.WMS( "OpenLayers WMS",
            "http://vmap0.tiles.osgeo.org/wms/vmap0?", {layers: 'basic'});

        var polygonLayer = new OpenLayers.Layer.Vector("Polygon Layer");

        map.addLayers([wmsLayer, polygonLayer]);
        map.addControl(new OpenLayers.Control.LayerSwitcher());
        map.addControl(new OpenLayers.Control.MousePosition());

        drawControls = {           
            polygon: new OpenLayers.Control.DrawFeature(polygonLayer,
                        OpenLayers.Handler.Polygon,{'featureAdded': setPolygonLocation})
        };

        for(var key in drawControls) {
            map.addControl(drawControls[key]);
        }

        map.setCenter(new OpenLayers.LonLat(0, 0), 3);

  }

function toggleControl() {
        for(key in drawControls) {
            var control = drawControls[key];
             control.activate();            
            }
      
    }

function setPolygonLocation(obj){

    var points = obj.geometry.toString();
    alert(points);
    $(document).getElementById("singleOutput").value = points;      

}
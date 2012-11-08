$(document).ready(function(){
  var pid = $('#pid').val();
  var page = decodeURI((RegExp('page' + '=' + '(.+?)(&|$)').exec($(this).attr('href'))||[,null])[1]);
  var tabs = $('#items, #metadata, #description, #history, #viewers, #manage');
  var content =  $('#div1');
  Drupal.behaviors.esdora_object_tabs = function(tab) {
    if(tabs.is(tab)) {
      $('a.fb_dynamic', content).click(function(event) {
	event.preventDefault();
      });
      $('a.fb_dynamic', content).fancybox({
	width: '100%',
	height: '100%',
	autoScale: false,
	type: 'iframe'
      });
    }
  }
  tabs.click(function(event){
    var selected = $(this);
    content.css('display','block');
    $('#viewers').css('display','none'); // Not totally sure if this is needed
    content.html('loading...');
    $.ajax({
      type:'GET',
      data: {'pid' : pid, 'tab': selected.attr('id'), 'page': page },
      success: function(result){
	content.html(result);
	tabs.css('background-color','#DDDDDD');
	selected.css('background-color','#FFFFFF');
	Drupal.attachBehaviors('#' + selected.attr('id'));
	return false;
      },
      dataType:"json"
    });
    return false;
  });
  // Items Pagers
  $('ul.pager a').live('click', function(event){
    event.preventDefault();
    content.css('display','block');
    $('#viewers').css('display','none');  // Not totally sure if this is needed
    content.html('loading...');
    $.ajax({
      type: 'GET',
      data: {'pid' : pid, 'tab': 'items', 'page': page },
      success: function(result){
	content.html(result);
	Drupal.attachBehaviors('#item');
	return false;
      },
      dataType:"json"
    });
    return false;
  });
});
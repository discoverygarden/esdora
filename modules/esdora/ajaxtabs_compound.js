var pid;
var ajaxUrl;
$(document).ready(function(){
  pid = $('#pid').val();
  ajaxUrl=Drupal.settings.basePath;
  ajaxUrl=ajaxUrl.concat("ajax/gettabs/compound");
  var page;
  //alert(ajaxUrl);
  $('#Items').click(function(){
    //alert('click items');
    $('#div1').css('display','block');
    $('#viewers').css('display','none');

    $('#div1').html('loading...');
    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid,'tab':'Items', 'page': page},
      success: function(result){
	$('#div1').html(result);
	$('#Metadata').css('background-color','#DDDDDD');
	$('#Items').css('background-color','#FFFFFF');
	$('#Description').css('background-color','#DDDDDD');
	$('#History').css('background-color','#DDDDDD');
	$('#Viewers').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');
	return false;
      },
      dataType:"json"
    });
    return false;

  });
  // Pagers
  $('ul.pager a').live('click', function(event){
    event.preventDefault();
    $('#div1').css('display','block');
    $('#viewers').css('display','none');
    $('#div1').html('loading...');
    page = decodeURI((RegExp('page' + '=' + '(.+?)(&|$)').exec($(this).attr('href'))||[,null])[1]);
    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid, 'tab':'Items', 'page': page },
      success: function(result){
	$('#div1').html(result);
	$('#Metadata').css('background-color','#DDDDDD');
	$('#Items').css('background-color','#FFFFFF');
	$('#Description').css('background-color','#DDDDDD');
	$('#History').css('background-color','#DDDDDD');
	$('#Viewers').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');
	return false;
      },
      dataType:"json"
    });
    return false;
  });
  $('#Metadata').click(function(){
    //alert('click metadata');
    $('#div1').css('display','block');
    $('#viewers').css('display','none');

    $('#div1').html('loading...');
    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid,'tab':'Metadata'},
      success: function(result){
	$('#div1').html(result);
	$('#Metadata').css('background-color','#FFFFFF');
	$('#Items').css('background-color','#DDDDDD');
	$('#Description').css('background-color','#DDDDDD');
	$('#History').css('background-color','#DDDDDD');
	$('#Viewers').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');

      },
      dataType:"json"
    });


  });
  $('#Description').click(function(){
    //alert('click Description');
    $('#div1').css('display','block');
    $('#viewers').css('display','none');

    $('#div1').html('loading...');
    var dWidth  = $(document).width()*.50;
    var dHeight = $(document).height()*.75;
    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid,'tab':'Description'},
      success: function(result){
	$('#div1').html(result);
	$('#Description').css('background-color','#FFFFFF');
	$('#Metadata').css('background-color','#DDDDDD');
	$('#Items').css('background-color','#DDDDDD');
	$('#History').css('background-color','#DDDDDD');
	$('#Viewers').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');
	$('a.fb_dynamic').live('click',function(e){
	  e.preventDefault();
	  $(this).fancybox({
	    'width':dWidth,
	    'height':dHeight,
	    'autoScale'         : false,
	    'type'          : 'iframe'
	  });

	});

      },
      dataType:"json"
    });

  });
  $('#History').click(function(){
    //alert('click History');
    $('#div1').css('display','block');
    $('#viewers').css('display','none');

    $('#div1').html('loading...');
    var dWidth  = $(document).width()*.50;
    var dHeight = $(document).height()*.75;

    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid,'tab':'History'},
      success: function(result){
	$('#div1').html(result);
	$('#History').css('background-color','#FFFFFF');
	$('#Metadata').css('background-color','#DDDDDD');
	$('#Items').css('background-color','#DDDDDD');
	$('#Description').css('background-color','#DDDDDD');
	$('#Viewers').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');
	$('a.fb_dynamic').live('click',function(e){
	  e.preventDefault();
	  $(this).fancybox({
	    'width':dWidth,
	    'height':dHeight,
	    'autoScale'         : false,
	    'type'          : 'iframe'
	  });

	});

      },
      dataType:"json"
    });

  });
  $('#Viewers').click(function(){
    $('#div1').css('display','block');
    $('#viewers').css('display','none');

    $('#div1').html('loading...');
    var dWidth  = $(document).width()*.70;
    var dHeight = $(document).height()*.85;
    $.ajax({
      type:'POST',
      url:ajaxUrl,
      data: {'pid' : pid,'tab':'Viewers'},
      success: function(result){
	$('#div1').html(result);
	$('#Viewers').css('background-color','#FFFFFF');
	$('#Items').css('background-color','#DDDDDD');
	$('#Description').css('background-color','#DDDDDD');
	$('#History').css('background-color','#DDDDDD');
	$('#Metadata').css('background-color','#DDDDDD');
	Drupal.attachBehaviors('#div1');
	$('a.fb_dynamic').live('click',function(e){
	  e.preventDefault();
	  $(this).fancybox({
	    'width':dWidth,
	    'height':dHeight,
	    'autoScale'         : false,
	    'transitionIn'       :   'elastic',
	    'transitionOut'  :   'elastic',
	    'type'          : 'iframe'
	  });
	  return false;

	});
      },
      dataType:"json"
    });

  });
})

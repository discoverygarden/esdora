var pid;
var ajaxUrl;
$(document).ready(function(){
	  pid = $('#pid').val();
	  ajaxUrl=Drupal.settings.basePath;
	  ajaxUrl=ajaxUrl.concat("ajax/gettabs");
	  alert(ajaxUrl);
	  $('.Items').click(function(){
		 $('#div1').html('loading...');
		 $.ajax({
				type:'POST',
				url:ajaxUrl,
				data: {'pid' : pid,'tab':'Items'},
				success: function(result){
					 $('#div1').html(result);
					 Drupal.attachBehaviors('#div1');
				},
				dataType:"json"
			});
			
		});
	  $('.Metadata').click(function(){
			 $('#div1').html('loading...');
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'Metadata'},
					success: function(result){
						 $('#div1').html(result);
						 Drupal.attachBehaviors('#div1');
					},
					dataType:"json"
				});
				
			});
	  $('.Description').click(function(){
		  $('#div1').html('loading...');
		  $.ajax({
				type:'POST',
				url:ajaxUrl,
				data: {'pid' : pid,'tab':'Description'},
				success: function(result){
					 $('#div1').html(result);
					 Drupal.attachBehaviors('#div1');
				},
				dataType:"json"
			});
			
		});
	  $('.History').click(function(){
			 $('#div1').html('loading...');
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'History'},
					success: function(result){
						 $('#div1').html(result);
						 Drupal.attachBehaviors('#div1');
					},
					dataType:"json"
				});
				
			});
	 
})
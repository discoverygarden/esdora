var pid;
var ajaxUrl;
$(document).ready(function(){
	  pid = $('#pid').val();
	  ajaxUrl=Drupal.settings.basePath;
	  ajaxUrl=ajaxUrl.concat("ajax/gettabs/compound");
	  //alert(ajaxUrl);
	 $('.Items').css('background-color','#C0C0C0');		 
	  $('.Items').click(function(){
		//alert('click items');
		 $('#div1').css('display','block');
		 $('#viewers').css('display','none');			 
			
		 $('#div1').html('loading...');
		 $.ajax({
				type:'POST',
				url:ajaxUrl,
				data: {'pid' : pid,'tab':'Items'},
				success: function(result){
					 $('#div1').html(result);
					 $('.Metadata').css('background-color','#E6E6E6');
					 $('.Items').css('background-color','#C0C0C0');
					 $('.Description').css('background-color','#E6E6E6');
					 $('.History').css('background-color','#E6E6E6');
					 $('.Viewers').css('background-color','#E6E6E6');
					 Drupal.attachBehaviors('#div1');
					 return false;
				},
				dataType:"json",
			});
			return false;
			
		});
	  $('.Metadata').click(function(){
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
						 $('.Metadata').css('background-color','#C0C0C0');
						 $('.Items').css('background-color','#E6E6E6');
						 $('.Description').css('background-color','#E6E6E6');
						 $('.History').css('background-color','#E6E6E6');
						 $('.Viewers').css('background-color','#E6E6E6');
						 Drupal.attachBehaviors('#div1');
						return false;
					},
					dataType:"json",
				});
				return false;
				
			});
	  $('.Description').click(function(){
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
					 $('.Description').css('background-color','#C0C0C0');					 
					 $('.Metadata').css('background-color','#E6E6E6');
					 $('.Items').css('background-color','#E6E6E6');
					 $('.History').css('background-color','#E6E6E6');
					 $('.Viewers').css('background-color','#E6E6E6');
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
				dataType:"json",
			});
			
		});
	  $('.History').click(function(){
			//alert('click History');
		  	 $('#div1').css('display','block');
			 $('#viewers').css('display','none');			 
			
			 $('#div1').html('loading...');
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'History'},
					success: function(result){
						 $('#div1').html(result);
						 $('.History').css('background-color','#C0C0C0');							
						 $('.Metadata').css('background-color','#E6E6E6');
						 $('.Items').css('background-color','#E6E6E6');
						 $('.Description').css('background-color','#E6E6E6');
						 $('.Viewers').css('background-color','#E6E6E6');
						 Drupal.attachBehaviors('#div1');
						
					},
					dataType:"json",
				});
				
			});
	  $('.Viewers').click(function(){
		  	 $('#div1').css('display','none');
			 $('#viewers').css('display','block');			 
			 $('.History').css('background-color','#E6E6E6');							
			 $('.Metadata').css('background-color','#E6E6E6');
			 $('.Items').css('background-color','#E6E6E6');
			 $('.Description').css('background-color','#E6E6E6');
			 $('.Viewers').css('background-color','#C0C0C0');
			 Drupal.attachBehaviors('#div1');						
		});	
		return false;
	 
	 
})

/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

Drupal.behaviors.fedoraRIStoreClientBehavior = function(){
    pid=Drupal.settings.fedoraRIStoreClient.pid;
    basepath=Drupal.settings.basePath;    
    ajaxUrl=basepath.concat("fedoraRIStoreClient_test");
    first = false;
    second = false;
    
    //checkboxes
    $('#check1').click(function ()
    {
        first = !first;
        
    });
    $('#check2').click(function ()
    {
        second= !second;
    });
    
    //submit button
    $('#submit').click(function()
    {
        $.ajax({
            type:'POST',
            url:ajaxUrl,
            data: {
                "pid":pid,
                "first":first,
                "second":second
            },
            success: function(result)
            {
                $('#display').html(result);                  		

            },
            dataType:"json"
        });

    });
    
    //refresh button
    $("#refresh").click(function(){
        url = basepath.concat("fedoraRIStoreClient/");
        url = url.concat(pid);
        window.location = url;
        
    });
    
    //new query
    $("#home").click(function(){
        
        window.location = basepath.concat("fedoraRIStoreClient");
    });
}


<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:s="http://www.w3.org/2001/sw/DataAccess/rf1/result" version="1.0" xmlns:php="http://php.net/xsl" exclude-result-prefixes="php">
 <!-- Red and White XSLT -->
 <xsl:variable name="BASEURL">
  <xsl:value-of select="$baseUrl"/>
 </xsl:variable>
 <xsl:variable name="PATH">
  <xsl:value-of select="$path"/>
 </xsl:variable>
 <xsl:variable name="thisPid" select="$collectionPid"/>
 <xsl:variable name="thisTitle" select="$collectionTitle"/>
 <xsl:variable name="size"  select="20"/>
 <xsl:variable name="page" select="$hitPage"/>
 <xsl:variable name="start" select="((number($page) - 1) * number($size)) + 1"/>
 <xsl:variable name="end" select="($start - 1) + number($size)"/>
  <xsl:variable name="cellsPerRow" select="3"/>
 <xsl:variable name="count" select="count(s:sparql/s:results/s:result)"/>
 
 
 
<xsl:template match="/">
 <xsl:if test="$count>0">
 
<table cellpadding="2" cellspacing="1" border="0" width="70%" >
  <tr style="border-bottom:1px solid #000; margin-bottom:4px;">
  <th colspan="3" align="center">Name</th>
  <th colspan="2" align="right">Type</th>
  </tr>
<xsl:apply-templates select="s:sparql/s:results"/>
</table>
 </xsl:if>
</xsl:template>

<xsl:template match="s:sparql/s:results">
   <xsl:for-each select="s:result">
   
  <tr style="border:0px;">
  <xsl:variable name='OBJECTURI' select="s:object/@uri"/>
  <xsl:variable name='CONTENTURI' select="s:content/@uri"/>
  <xsl:variable name='CONTENTMODEL' select="substring-after($CONTENTURI,'/')"/>
  <xsl:variable name='PID' select="substring-after($OBJECTURI,'/')"/>
  <xsl:variable name="newTitle">
	  <xsl:call-template name="replace-string">
	   <xsl:with-param name="text" select="s:title"/>
	   <xsl:with-param name="from" select="'_'"/>
	   <xsl:with-param name="to" select="' '"/>
	  </xsl:call-template>
  </xsl:variable>
  
  <xsl:variable name="cleanTitle">
    <xsl:value-of select="php:functionString('fedora_repository_urlencode_string', $newTitle)"/>
  </xsl:variable>
  
  <xsl:variable name="linkUrl">
   <xsl:choose>
    <xsl:when test="($CONTENTMODEL='islandora:collectionCModel')"> <!-- ? the test result not matter? -->
     <xsl:value-of select="$BASEURL"/>/fedora/repository/<xsl:copy-of select="$PID"/>/-/<xsl:value-of select="$cleanTitle"/>
    </xsl:when>
    <xsl:otherwise>
     <xsl:value-of select="$BASEURL"/>/fedora/repository/<xsl:copy-of select="$PID"/>/-/<xsl:value-of select="$cleanTitle"/>
     </xsl:otherwise>
   </xsl:choose>
   <xsl:value-of select="s:content"/>
  </xsl:variable>
  
  <xsl:variable name ="imagUrl">
	<xsl:choose>
	<xsl:when test ="($CONTENTMODEL='esdora:defaultCollectionCModel')">  <!-- this is the data object that has this model -->
  	<xsl:value-of select="$BASEURL"/>/sites/default/files/islandora_img/folder-2-icon.png
	</xsl:when>
	<xsl:otherwise>
  	<xsl:value-of select="$BASEURL"/>/sites/default/files/islandora_img/file-2-icon.png
	</xsl:otherwise>
	</xsl:choose>  
  </xsl:variable>

  <xsl:variable name="childCount">
    <xsl:value-of select="php:functionString('get_child_count', $PID)"/>
  </xsl:variable>

  <td colspan="3">   
    <a>
    <xsl:attribute name="href"><xsl:value-of select="$linkUrl"/>
    </xsl:attribute>
    <xsl:value-of select="$newTitle" disable-output-escaping="yes" />
   </a>
    <br clear="all"/>
  </td>
  <!--
  <td halign="left" colspan="1">
  	<form>
  		<xsl:attribute name="action"><xsl:value-of select="$linkUrl"/></xsl:attribute>
  		<xsl:attribute name="method"><xsl:value-of select="get"/></xsl:attribute>
  		<input type="submit" value="view details"/>
  	</form>
  </td>
   
   -->
   
   <td colspan="2">
   	<a>
    <xsl:attribute name="href"><xsl:value-of select="$linkUrl"/>
    </xsl:attribute>
    
   <img width="16" height="16">
    <xsl:attribute name="src"><xsl:value-of select="$imagUrl"/>
    </xsl:attribute>
	<xsl:attribute name="alt"><xsl:value-of select="$newTitle" disable-output-escaping="yes"/>
    </xsl:attribute>
   </img>
   </a>
  	<xsl:value-of select="$childCount"></xsl:value-of>
   	<!-- 
   	<xsl:value-of select="$BASEURL"/>/fedora/risearch?lang=itql&format=Sparql&query=select+count+%28select+%24object+from+%3C%23ri%3E+where+%24object+%3Cfedora-rels-ext%3AisMemberOf%3E+%3Cinfo%3Afedora%2F<xsl:copy-of select="$PID"/>%3E%29+from+%3C%23ri%3E+where+%24s+%24p+%24o
   	-->
   </td>
    </tr>
   <br clean="all"/>
   </xsl:for-each>
</xsl:template>

 <xsl:template name="replace-string">
  <xsl:param name="text"/>
  <xsl:param name="from"/>
  <xsl:param name="to"/>

  <xsl:choose>
   <xsl:when test="contains($text, $from)">

    <xsl:variable name="before" select="substring-before($text, $from)"/>
    <xsl:variable name="after" select="substring-after($text, $from)"/>
    <xsl:variable name="prefix" select="concat($before, $to)"/>

    <xsl:value-of select="$before"/>
    <xsl:value-of select="$to"/>
    <xsl:call-template name="replace-string">
     <xsl:with-param name="text" select="$after"/>
     <xsl:with-param name="from" select="$from"/>
     <xsl:with-param name="to" select="$to"/>
    </xsl:call-template>
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="$text"/>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>


</xsl:stylesheet>
<?xml version="1.0" encoding="UTF-8"?>
<!-- $Id: demoFoxmlToLucene.xslt 5734 2006-11-28 11:20:15Z gertsp $ -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exts="xalan://dk.defxws.fedoragsearch.server.GenericOperationsImpl" xmlns:islandora-exts="xalan://ca.upei.roblib.DataStreamForXSLT" exclude-result-prefixes="exts islandora-exts" xmlns:zs="http://www.loc.gov/zing/srw/" xmlns:foxml="info:fedora/fedora-system:def/foxml#" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" xmlns:tei="http://www.tei-c.org/ns/1.0" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#" xmlns:fedora="info:fedora/fedora-system:def/relations-external#" xmlns:relsModel="info:fedora/fedora-system:def/model#" xmlns:rel="info:fedora/fedora-system:def/relations-external#" xmlns:fractions="http://vre.upei.ca/fractions/" xmlns:compounds="http://vre.upei.ca/compounds/" xmlns:critters="http://vre.upei.ca/critters/" xmlns:fgdc="http://www.fgdc.gov/metadata/csdgm/index_html" xmlns:fedora-model="info:fedora/fedora-system:def/model#" xmlns:uvalibdesc="http://dl.lib.virginia.edu/bin/dtd/descmeta/descmeta.dtd" xmlns:uvalibadmin="http://dl.lib.virginia.edu/bin/admin/admin.dtd/">
	
	<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
	
	<!--
		This xslt stylesheet generates the Solr doc element consisting of field elements
		from a FOXML record. The PID field is mandatory.
		Options for tailoring:
		- generation of fields from other XML metadata streams than DC
		- generation of fields from other datastream types than XML
		- from datastream by ID, text fetched, if mimetype can be handled
		currently the mimetypes text/plain, text/xml, text/html, application/pdf can be handled.
	-->
	
	<xsl:param name="REPOSITORYNAME" select="repositoryName"/>
	<xsl:param name="FEDORASOAP" select="repositoryName"/>
	<xsl:param name="FEDORAUSER" select="repositoryName"/>
	<xsl:param name="FEDORAPASS" select="repositoryName"/>
	<xsl:param name="TRUSTSTOREPATH" select="repositoryName"/>
	<xsl:param name="TRUSTSTOREPASS" select="repositoryName"/>
	<xsl:variable name="PID" select="/foxml:digitalObject/@PID"/>
	<xsl:variable name="docBoost" select="1.4*2.5"/> <!-- or any other calculation, default boost is 1.0 -->
	
	<xsl:template match="/">
		<add> 
			<doc> 
				<xsl:attribute name="boost">
					<xsl:value-of select="$docBoost"/>
				</xsl:attribute>
				<!-- The following allows only active demo FedoraObjects to be indexed. -->
				<xsl:if test="foxml:digitalObject/foxml:objectProperties/foxml:property[@NAME='info:fedora/fedora-system:def/model#state' and @VALUE='Active']">
					<xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP'] or foxml:digitalObject/foxml:datastream[@ID='DS-COMPOSITE-MODEL'])">
						<xsl:if test="starts-with($PID,'')">
							<xsl:apply-templates mode="activeDemoFedoraObject"/>
						</xsl:if>
					</xsl:if>
				</xsl:if>
			</doc>
		</add>
	</xsl:template>
	
	<xsl:template match="/foxml:digitalObject" mode="activeDemoFedoraObject">
		<field name="PID" boost="2.5">
			<xsl:value-of select="$PID"/>
		</field>
		<xsl:for-each select="foxml:objectProperties/foxml:property">
			<field>
				<xsl:attribute name="name"> 
					<xsl:value-of select="concat('fgs.', substring-after(@NAME,'#'))"/>
				</xsl:attribute>
				<xsl:value-of select="@VALUE"/>
			</field>
		</xsl:for-each>
		<xsl:for-each select="foxml:datastream/foxml:datastreamVersion[last()]/foxml:xmlContent/oai_dc:dc/*">
			<xsl:if test="text() [normalize-space(.) ]"><!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('dc.', substring-after(name(),':'))"/>
					</xsl:attribute>
					<xsl:value-of select="text()"/>
				</field>
			</xsl:if>
		</xsl:for-each>	
		<xsl:for-each select="foxml:datastream/foxml:datastreamVersion[last()]/foxml:xmlContent/reference/*">
			<field>
				<xsl:attribute name="name">
					<xsl:value-of select="concat('refworks.', name())"/>
				</xsl:attribute>
				<xsl:value-of select="text()"/>
			</field>
		</xsl:for-each>
		
		
		<xsl:for-each select="foxml:datastream[@ID='RIGHTSMETADATA']/foxml:datastreamVersion[last()]/foxml:xmlContent//access/human/person">
			<field>
				<xsl:attribute name="name">access.person</xsl:attribute>
				<xsl:value-of select="text()"/>
			</field>			
		</xsl:for-each>
		<xsl:for-each select="foxml:datastream[@ID='RIGHTSMETADATA']/foxml:datastreamVersion[last()]/foxml:xmlContent//access/human/group">
			<field>
				<xsl:attribute name="name">access.group</xsl:attribute>
				<xsl:value-of select="text()"/>
			</field>			
		</xsl:for-each>
		
		
		<xsl:for-each select="foxml:datastream[@ID='RELS-EXT']/foxml:datastreamVersion[last()]/foxml:xmlContent//rdf:description/*">
			<field>
				<xsl:attribute name="name">
					<xsl:value-of select="concat('rels.', substring-after(name(),':'))"/>
				</xsl:attribute>
				<xsl:variable name="VALUE"><xsl:value-of select="@rdf:resource"/></xsl:variable>
				<xsl:value-of select="substring-after($VALUE,'/')"/>
			</field>
		</xsl:for-each>
		<xsl:for-each select="foxml:datastream[@ID='RELS-EXT']/foxml:datastreamVersion[last()]/foxml:xmlContent//rdf:description/relsModel:hasModel">
			<field>
				<xsl:attribute name="name">
					<xsl:value-of select="concat('rels.', name())"/>
				</xsl:attribute>
				<xsl:variable name="VALUE"><xsl:value-of select="@rdf:resource"/></xsl:variable>
				<xsl:value-of select="substring-after($VALUE,'/')"/>
			</field>
		</xsl:for-each>
				
		<!-- FGDC Start -->		
		<xsl:for-each select="foxml:datastream[@ID='FGDC.xml']/foxml:datastreamVersion[last()]">
		<xsl:call-template name="fgdc"/>
			<!--only call this if the mods stream exists-->
	</xsl:for-each>
	</xsl:template>
	<!-- template to call for FGDC streams  This calls an external function getXMLDatastreamASNodeList this method is in a class developed at UPEI that you have to drop into your gsearch WEB-INF classes directory
		The xpath is this template are probably not valid just examples to get started with  The mods template etc are just left here as an example as well.-->
	<xsl:template name="fgdc">
		<xsl:variable name="FGDC_STREAM" select="islandora-exts:getXMLDatastreamASNodeList($PID, $REPOSITORYNAME, 'FGDC.xml', $FEDORASOAP, $FEDORAUSER, $FEDORAPASS, $TRUSTSTOREPATH, $TRUSTSTOREPASS)"/>
		
		<!--these are the fields we want to index
			1. Title: metadata.idinfo.citation.citeinfo.title
			2. Creator: metadata.idindo.citation.citeinfo.origin
			3. Summary: metadata.idindo.descript.abstract
			4. Start Time: metadata.idinfo.timeperd.timeinfo.rngdates.begdate
			5. End Time: metadata.idinfo.timeperd.timeinfo.rngdates.enddate
			6. BD Box West: metadata.idinfo.spdom.bounding.westbc
			7. BD Box East: metadata.idinfo.spdom.bounding.eastbc
			8. BD Box North: metadata.idinfo.spdom.bounding.northbc
			9. BD Box South: metadata.idinfo.spdom.bounding.southbc
			10. themekt: metadata.idinfo.keywords.theme.themekt
			11. themekey: metadata.idinfo.keywords.theme.themekey
			12. placekt: metadata.idinfo.keywords.theme.placekt
			13. placekey: metadata.idinfo.keywords.theme.placekey-->
		
		<xsl:for-each select="$FGDC_STREAM//idinfo/timeperd/timeinfo/rngdates/begdate">
			<xsl:if test="text() [normalize-space(.) ]">
				<!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'start_time')"/>
					</xsl:attribute>
				<!--	<xsl:value-of select="text()"/> -->
				        <xsl:variable name="suffix">T00:00:00Z</xsl:variable>
                                        <xsl:variable name="date" select="text()"/>
                                        <xsl:variable name="lenth" select="string-length($date)"/>
                                        <xsl:if test="$lenth = 8 and not(contains($date, '-')) and not(contains($date, '/'))">
                                            <!-- assuming a pattern of 19700101 -->
                                            <xsl:variable name="year">
                                            <xsl:value-of select="substring($date, 1,4)"/>
                                            </xsl:variable>
                                            <xsl:variable name="month">
                                            <xsl:value-of select="substring($date, 5,2)"/>
                                            </xsl:variable>
                                            <xsl:variable name="day">
                                            <xsl:value-of select="substring($date, 7,2)"/>
                                            </xsl:variable>
                                            <xsl:value-of select="concat($year, '-', $month, '-', $day, $suffix)"/>
                                        </xsl:if>
                                        <xsl:if test="$lenth = 10 and contains($date, '-')">
                                            <!-- assuming a pattern of 1970-01-01 -->
                			    <xsl:value-of select="concat($date, $suffix)"/>
            				</xsl:if>
            				<xsl:if test="$lenth = 10 and contains($date, '/')">
                			    <!-- assuming a pattern of 1970/01/01 -->
                			    <xsl:variable name="year">
                    			    <xsl:value-of select="substring($date, 1,4)"/>
                			    </xsl:variable>
                			    <xsl:variable name="month">
                    			    <xsl:value-of select="substring($date, 6,2)"/>
                			    </xsl:variable>
                			    <xsl:variable name="day">
                    			    <xsl:value-of select="substring($date, 9,2)"/>
                			    </xsl:variable>
                	                    <xsl:value-of select="concat($year, '-', $month, '-', $day, $suffix)"/>
            				</xsl:if>
            				<xsl:if test="$lenth = 4">
                			    <!-- assuming a pattern of 1970 -->
                			    <xsl:value-of select="concat($date, '-', '01', '-', '01', $suffix)"/>
            				</xsl:if>
				</field>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="$FGDC_STREAM//idinfo/timeperd/timeinfo/rngdates/enddate">
			<xsl:if test="text() [normalize-space(.) ]">
				<!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'end_time')"/>
					</xsl:attribute>
				<!--	<xsl:value-of select="text()"/> -->
				        <xsl:variable name="suffix">T00:00:00Z</xsl:variable>
                                        <xsl:variable name="date" select="text()"/>
                                        <xsl:variable name="lenth" select="string-length($date)"/>
                                        <xsl:if test="$lenth = 8 and not(contains($date, '-')) and not(contains($date, '/'))">
                                            <!-- assuming a pattern of 19700101 -->
                                            <xsl:variable name="year">
                                            <xsl:value-of select="substring($date, 1,4)"/>
                                            </xsl:variable>
                                            <xsl:variable name="month">
                                            <xsl:value-of select="substring($date, 5,2)"/>
                                            </xsl:variable>
                                            <xsl:variable name="day">
                                            <xsl:value-of select="substring($date, 7,2)"/>
                                            </xsl:variable>
                                            <xsl:value-of select="concat($year, '-', $month, '-', $day, $suffix)"/>
                                        </xsl:if>
                                        <xsl:if test="$lenth = 10 and contains($date, '-')">
                                            <!-- assuming a pattern of 1970-01-01 -->
                			    <xsl:value-of select="concat($date, $suffix)"/>
            				</xsl:if>
            				<xsl:if test="$lenth = 10 and contains($date, '/')">
                			    <!-- assuming a pattern of 1970/01/01 -->
                			    <xsl:variable name="year">
                    			    <xsl:value-of select="substring($date, 1,4)"/>
                			    </xsl:variable>
                			    <xsl:variable name="month">
                    			    <xsl:value-of select="substring($date, 6,2)"/>
                			    </xsl:variable>
                			    <xsl:variable name="day">
                    			    <xsl:value-of select="substring($date, 9,2)"/>
                			    </xsl:variable>
                	                    <xsl:value-of select="concat($year, '-', $month, '-', $day, $suffix)"/>
            				</xsl:if>
            				<xsl:if test="$lenth = 4">
                			    <!-- assuming a pattern of 1970 -->
                			    <xsl:value-of select="concat($date, '-', '01', '-', '01', $suffix)"/>
            				</xsl:if>
				</field>
			</xsl:if>
		</xsl:for-each>

 		<xsl:for-each select="$FGDC_STREAM//idinfo/spdom/bounding/westbc">
                        <xsl:if test="text() [normalize-space(.) ]">
                                <!--don't bother with empty space-->
                                <field>
                                        <xsl:attribute name="name">
                                                <xsl:value-of select="concat('fgdc.', 'west')"/>
                                        </xsl:attribute>
                                        <xsl:value-of select="text()"/>
                                </field>
                        </xsl:if>
                </xsl:for-each>

 		<xsl:for-each select="$FGDC_STREAM//idinfo/spdom/bounding/northbc">
                        <xsl:if test="text() [normalize-space(.) ]">
                                <!--don't bother with empty space-->
                                <field>
                                        <xsl:attribute name="name">
                                                <xsl:value-of select="concat('fgdc.', 'north')"/>
                                        </xsl:attribute>
                                        <xsl:value-of select="text()"/>
                                </field>
                        </xsl:if>
                </xsl:for-each>

 		<xsl:for-each select="$FGDC_STREAM//idinfo/spdom/bounding/eastbc">
                        <xsl:if test="text() [normalize-space(.) ]">
                                <!--don't bother with empty space-->
                                <field>
                                        <xsl:attribute name="name">
                                                <xsl:value-of select="concat('fgdc.', 'east')"/>
                                        </xsl:attribute>
                                        <xsl:value-of select="text()"/>
                                </field>
                        </xsl:if>
                </xsl:for-each>
 		<xsl:for-each select="$FGDC_STREAM//idinfo/spdom/bounding/southbc">
                        <xsl:if test="text() [normalize-space(.) ]">
                                <!--don't bother with empty space-->
                                <field>
                                        <xsl:attribute name="name">
                                                <xsl:value-of select="concat('fgdc.', 'south')"/>
                                        </xsl:attribute>
                                        <xsl:value-of select="text()"/>
                                </field>
                        </xsl:if>
                </xsl:for-each>


		<xsl:for-each select="$FGDC_STREAM//idinfo/citation/citeinfo/title">
			<xsl:if test="text() [normalize-space(.) ]">
				<!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'title')"/>
					</xsl:attribute>
					<xsl:value-of select="text()"/>
					<xsl:text> </xsl:text>
				</field>
			</xsl:if>
			
		</xsl:for-each>
		<xsl:for-each select="$FGDC_STREAM//idinfo/citation/citeinfo/origin">
			<xsl:if test="text() [normalize-space(.) ]">
				<!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'creator')"/>
					</xsl:attribute>
					<xsl:value-of select="normalize-space(text())"/>
				</field>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="$FGDC_STREAM//idinfo/descript/abstract">
			<xsl:if test="text() [normalize-space(.) ]">
				<!--don't bother with empty space-->
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'summary')"/>
					</xsl:attribute>
					<xsl:value-of select="text()"/>
				</field>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="$FGDC_STREAM//placekey">
			
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'place_keywords')"/>
					</xsl:attribute>
					<xsl:value-of select="text()"/>
				</field>
		</xsl:for-each>
		
		<xsl:for-each select="$FGDC_STREAM//themekey">
			
				<field>
					<xsl:attribute name="name">
						<xsl:value-of select="concat('fgdc.', 'theme_keywords')"/>
					</xsl:attribute>
					<xsl:value-of select="text()"/>
				</field>
		</xsl:for-each>
	</xsl:template>
		
</xsl:stylesheet>
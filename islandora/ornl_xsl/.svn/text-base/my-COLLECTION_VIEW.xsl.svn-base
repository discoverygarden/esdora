<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:s="http://www.w3.org/2001/sw/DataAccess/rf1/result" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="/">
    <xsl:variable name="BASEURL">
      <xsl:value-of select="$baseUrl"></xsl:value-of>
    </xsl:variable>
    <xsl:variable name="collTitle" select="/s:sparql/s:results/s:result/s:collTitle"></xsl:variable>
    <xsl:variable name="collDesc" select="/s:sparql/s:results/s:result/s:collDesc"></xsl:variable>
    <center>
      <font face="arial,helvetica">

        <h2>
          <xsl:value-of select="$collTitle"></xsl:value-of>
          <br></br>
          <i>
            <xsl:value-of select="$collDesc"></xsl:value-of>
          </i>
        </h2>
      </font>
    </center>

    <hr size="1"></hr>
    <center>
      <table border="0" cellpadding="5">
        <xsl:for-each select="/s:sparql/s:results/s:result">
          <xsl:variable name="pid" select="substring-after(s:member/@uri, &apos;/&apos;)"></xsl:variable>
          <tr>
            <td>
              <center>
                <a>

                  <xsl:attribute name="href">
                    <xsl:value-of select="$BASEURL"></xsl:value-of>
                    <xsl:text>/fedora/repository/</xsl:text>
                    <xsl:value-of select="$pid"></xsl:value-of>
                    <xsl:text>/FULL_SIZE</xsl:text>
                  </xsl:attribute>
                  <img height="120" width="160">
                    <xsl:attribute name="src">

                      <xsl:value-of select="$BASEURL"></xsl:value-of>
                      <xsl:text>/fedora/repository/</xsl:text>
                      <xsl:value-of select="$pid"></xsl:value-of>
                      <xsl:text>/MEDIUM_SIZE</xsl:text>
                    </xsl:attribute>
                  </img>
                  <br></br>
                      ( Full Size )
                    </a>

              </center>
            </td>
            <td>
              <b>
                <a>
                  <xsl:attribute name="href">
                    <xsl:value-of select="$BASEURL"></xsl:value-of>
                    <xsl:text>/fedora/repository/</xsl:text>

                    <xsl:value-of select="$pid"></xsl:value-of>/-/<xsl:value-of select="s:memberTitle"></xsl:value-of>
                  </xsl:attribute>
                  <xsl:value-of select="s:memberTitle"></xsl:value-of>
                </a>
              </b>
              <br></br>
              <xsl:value-of select="s:memberDesc"></xsl:value-of>
            </td>

          </tr>
        </xsl:for-each>
      </table>
    </center>
  </xsl:template>
</xsl:stylesheet>

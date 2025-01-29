<?xml version="1.0" encoding="iso-8859-1"?>
<!-- This is the Deep Space 6 stylesheet to convert DocBook articles to xhtml
     See http://www.deepspace6.net for info. -->
<!-- copy+adjust 2025 for ipv6calc local -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <!-- don't re-invent the wheel, we simply customize the standard 
       docbook stylesheet  -->  
  <xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets/xhtml/docbook.xsl"/>
  <xsl:output encoding="iso-8859-1"/>     

  <!-- use this CSS stylesheet -->
  <xsl:param name="html.stylesheet" select="'dbprojects.css'"/>
  <!-- it's CSS -->
  <xsl:param name="html.stylesheet.type">text/css</xsl:param>
  <!-- entry creates a class attribute -->
  <xsl:param name="entry.propagates.style" select="1"/>
  
  <!-- uncomment the following line to suppress TOC generation -->
  <!-- 
  <xsl:param name="generate.toc" select="0"/>
  -->
  
  <!-- we don't like the separator after the title, so we ovverride
       the default behaviour -->
  <xsl:template name="article.titlepage.separator">
    <!-- do nothing -->
  </xsl:template>
  
  <!-- we could put some header text in this templatet -->
  <xsl:template name="user.header.content">
    <xsl:param name="node" select="."/>
    <!-- insert here -->
  </xsl:template>

  <!-- we don't like the added <hr/> after orgname, override it -->
  <xsl:template match="orgname" mode="titlepage.mode">
    <span xmlns="http://www.w3.org/1999/xhtml" class="{name(.)}">
      <xsl:apply-templates mode="titlepage.mode"/>
    </span>
  </xsl:template>

</xsl:stylesheet>
<!--
vi:et ts=2 sw=2
-->

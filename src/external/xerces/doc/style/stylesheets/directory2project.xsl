<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:param name="base"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="directory">
    <project>
      <xsl:apply-templates/>
    </project>
  </xsl:template>

  <xsl:template match="entry">

    <!--xsl:if test="@directory!='true'"-->
      <resource source="{@href}" target="{$base}{@href}"/>
    <!--/xsl:if-->
<!-- don't copy subdir to avoid copying proprietary CVS files
    <xsl:if test="@directory='true'">
      <process source="{@href}" producer="directory">
        <processor name="xslt">
          <parameter name="stylesheet" value="sbk:/style/stylesheets/directory2project.xsl"/>
          <parameter name="base" value="{$base}{@href}"/>
        </processor>
      </process>
    </xsl:if>
-->    
  </xsl:template>

</xsl:stylesheet>
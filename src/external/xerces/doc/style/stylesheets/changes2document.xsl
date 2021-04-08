<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="changes">
    <s1 title="{@title}">
      <xsl:apply-templates/>
    </s1>
  </xsl:template>

  <xsl:template match="release">
    <s2 title="Release {@version} {@date}">
      <br/><xsl:apply-templates/>
    </s2>
  </xsl:template>

  <xsl:template match="features">
    <s3 title="Features">
      <ul><xsl:apply-templates/></ul>
    </s3>
  </xsl:template>

  <xsl:template match="fixes">
    <s3 title="Bugs fixed">
      <ul><xsl:apply-templates/></ul>
    </s3>
  </xsl:template>

  <xsl:template match="feat|fix">
    <li><xsl:apply-templates/></li>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
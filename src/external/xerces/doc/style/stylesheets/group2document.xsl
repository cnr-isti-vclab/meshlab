<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="group">
    <s1 title="{@title}">
      <xsl:apply-templates/>
    </s1>
  </xsl:template>

  <xsl:template match="entry">
    <s2 title="{@title}">
      <p>
        Read the <link idref="{@id}"><xsl:value-of select="@title"/></link>
        document or jump directly to:
      </p>
      <ul>
        <xsl:apply-templates/>
      </ul>
    </s2>
  </xsl:template>

  <xsl:template match="voice">
    <li><link idref="{ancestor::*/@id}" anchor="faq-{position()}"><xsl:apply-templates/></link></li>
  </xsl:template>

</xsl:stylesheet>
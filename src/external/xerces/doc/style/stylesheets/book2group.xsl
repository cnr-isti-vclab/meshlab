<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:param name="id"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="book">
    <group title="{group[attribute::id=$id]/@label}">
      <xsl:apply-templates select="group[attribute::id=$id]/entry"/>
    </group>
  </xsl:template>

  <xsl:template match="entry">
    <entry id="{@id}" title="{document(concat('sbk:/sources/',@source))/faqs/@title}">
      <xsl:apply-templates select="document(concat('sbk:/sources/',@source))/faqs/faq"/>
    </entry>
  </xsl:template>

  <xsl:template match="faq">
    <voice>
      <xsl:if test="string-length(@title)=0">
        <xsl:value-of select="q"/>
      </xsl:if>
      <xsl:if test="string-length(@title)>0">
        <xsl:value-of select="@title"/>
      </xsl:if>
    </voice>
  </xsl:template>

</xsl:stylesheet>
<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:param name="label"/>

  <xsl:template match="/">
    <image width="456" height="35" bgcolor="0086b2">
      <xsl:apply-templates/>
    </image>
  </xsl:template>

  <xsl:template match="s1|faqs|changes">
      <xsl:variable name="title">
        <xsl:if test="string-length(@title)=0">
          <xsl:value-of select="$label"/>
        </xsl:if>
        <xsl:if test="string-length(@title)>0">
          <xsl:value-of select="@title"/>
        </xsl:if>
      </xsl:variable>

      <text font="Arial" size="29" x="454" y="8" halign="right" valign="top" color="004080"
            text="{$title}"/>
      <text font="Arial" size="29" x="452" y="6" halign="right" valign="top" color="ffffff"
            text="{$title}"/>
  </xsl:template>

</xsl:stylesheet>
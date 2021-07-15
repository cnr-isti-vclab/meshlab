<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="context">
    <image source="sbk:/style/graphics/footer.gif">
      <xsl:apply-templates/>
    </image>
  </xsl:template>

  <xsl:template match="parameter">
    <xsl:if test="@name='copyright'">
      <text font="Arial Bold" size="11" x="124" y="6" style="plain"
            halign="left" valign="top" color="666699"
            text="Copyright &#169; {@value}. All Rights Reserved."/>
      <text font="Arial Bold" size="11" x="123" y="5" style="plain"
            halign="left" valign="top" color="333366"
            text="Copyright &#169; {@value}. All Rights Reserved."/>
      <text font="Arial Bold" size="11" x="122" y="4" style="plain"
            halign="left" valign="top" color="ffffff"
            text="Copyright &#169; {@value}. All Rights Reserved."/>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
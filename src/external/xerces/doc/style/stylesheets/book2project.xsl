<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- match the root book element -->
  <xsl:template match="book">
    <project>

      <parameter name="copyright" value="{@copyright}"/>

      <!-- copy all resources to the targets -->
      <process source="sbk:/style/resources/" producer="directory">
        <processor name="xslt">
          <parameter name="stylesheet" value="sbk:/style/stylesheets/directory2project.xsl"/>
          <parameter name="base" value="resources/"/>
        </processor>
      </process>

      <xsl:apply-templates/>

    </project>
  </xsl:template>

<!-- ********************************************************************** -->
<!-- CREATE THE TARGET HTML -->
<!-- ********************************************************************** -->

  <xsl:template match="document">

    <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="@id"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>
  </xsl:template>

  <xsl:template match="hidden">

    <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>
  </xsl:template>

  <xsl:template match="faqs">

    <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="@id"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/faqs2document.xsl"/>
      </processor>
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>

  </xsl:template>

  <xsl:template match="changes">

    <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="@id"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/changes2document.xsl"/>
      </processor>
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>

  </xsl:template>

  <xsl:template match="group">
    <xsl:apply-templates/>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="@id"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

    <create source="" target="{@id}.html" producer="project" printer="html">
      <parameter name="id" value="{@id}"/>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/book2group.xsl"/>
      </processor>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/group2document.xsl"/>
      </processor>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>

  </xsl:template>

   <xsl:template match="container">

	<xsl:apply-templates/>

   <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>


    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="@id"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>


    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>

  </xsl:template>

  <xsl:template match="entry">

    <process source="{@source}" producer="parser">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2project.xsl"/>
      </processor>
    </process>

    <xsl:call-template name="header">
      <xsl:with-param name="id"     select="@id"/>
      <xsl:with-param name="source" select="@source"/>
      <xsl:with-param name="label"  select="@label"/>
    </xsl:call-template>

    <create source="{@source}" target="{@id}.html" producer="parser" printer="html">
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/faqs2document.xsl"/>
      </processor>
      <processor name="xslt">
        <parameter name="id" value="{@id}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/document2html.xsl"/>
      </processor>
    </create>

  </xsl:template>

  <xsl:template match="external">

    <xsl:call-template name="labels">
      <xsl:with-param name="id" select="concat('ext-',position())"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

  </xsl:template>

  <xsl:template match="section">

    <xsl:call-template name="sect-label">
      <xsl:with-param name="id" select="concat('sect-',position())"/>
      <xsl:with-param name="label" select="@label"/>
    </xsl:call-template>

  </xsl:template>

<!-- ********************************************************************** -->
<!-- NAMED TEMPLATES -->
<!-- ********************************************************************** -->

<!-- Generate the doument header image -->
  <xsl:template name="header">
    <xsl:param name="id"/>
    <xsl:param name="source"/>
    <xsl:param name="label"/>

    <create source="{$source}" target="graphics/{$id}-header.jpg" producer="parser" printer="image">
      <processor name="xslt">
        <parameter name="label" value="{$label}"/>
        <parameter name="stylesheet" value="sbk:/style/stylesheets/any2header.xsl"/>
      </processor>
    </create>
  </xsl:template>

<!-- Generate the three rollover label images -->
  <xsl:template name="labels">
    <xsl:param name="id"/>
    <xsl:param name="label"/>

    <create source="" target="graphics/{$id}-label-1.jpg" producer="context" printer="image">
      <parameter name="label" value="{$label}"/>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/context2label.xsl"/>
        <parameter name="image" value="sbk:/style/graphics/button-a.gif"/>
        <parameter name="color" value="ffffff"/>
      </processor>
    </create>

    <create source="" target="graphics/{$id}-label-2.jpg" producer="context" printer="image">
      <parameter name="label" value="{$label}"/>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/context2label.xsl"/>
        <parameter name="image" value="sbk:/style/graphics/button-b.gif"/>
        <parameter name="color" value="ffff00"/>
      </processor>
    </create>

    <create source="" target="graphics/{$id}-label-3.jpg" producer="context" printer="image">
      <parameter name="label" value="{$label}"/>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/context2label.xsl"/>
        <parameter name="image" value="sbk:/style/graphics/button-b.gif"/>
        <parameter name="color" value="ffffff"/>
      </processor>
    </create>
  </xsl:template>

<!-- Generate one bold section label -->
  <xsl:template name="sect-label">
    <xsl:param name="id"/>
    <xsl:param name="label"/>

    <create source="" target="graphics/{$id}-label.jpg" producer="context" printer="image">
      <parameter name="label" value="{$label}"/>
      <processor name="xslt">
        <parameter name="stylesheet" value="sbk:/style/stylesheets/context2section.xsl"/>
        <parameter name="color" value="ffffff"/>
      </processor>
    </create>
  </xsl:template>

</xsl:stylesheet>

<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:output 
     method="html" 
     doctype-public="XSLT-compat"
     encoding="UTF-8"
     />

  <xsl:param name="filename"/>
  <xsl:param name="lastchange"/>  
  <xsl:param name="group" select="/page/@group" />  
  
  <xsl:template match="node()|@*">
    <xsl:copy><xsl:apply-templates select="@* | node()" /></xsl:copy>
  </xsl:template>

  <xsl:template match="dlink">
    <a href="{@href}"><xsl:value-of select="substring-after(@href,'http://savannah.nongnu.org/download/windstille/')" /></a>
  </xsl:template> 

  <xsl:template match="dlink-full">
    <a href="{@href}"><xsl:value-of select="@href" /></a>
  </xsl:template> 

  <xsl:template match="page">
    <html>
      <head>
        <title>Windstille - A Jump'n Shoot Game</title>
        <link rel="stylesheet" type="text/css" href="default.css" />
        <link rel="icon" href="images/favicon.png" type="image/png" />
        <meta name="google-site-verification" content="GJ6Jvfp0OlkCgQWBqTAXEmc3R_2nr5FDboaSUv2tugc" />
      </head>

      <body>
        <div style="float: left; margin-top: 1em;">
          <a href="http://flattr.com/thing/5654/Windstille-A-sidescrolling-2d-sci-fi-action-adventure" target="_blank">
            <img src="http://api.flattr.com/button/button-static-50x60.png" title="Flattr this" border="0" /></a>
        </div>

        <div style="float: right; margin-top: 1em;">
          <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
            <input type="hidden" name="cmd" value="_xclick" />
            <input type="hidden" name="business" value="grumbel@gmx.de" />
            <input type="hidden" name="item_name" value="Windstille donation" />
            <input type="hidden" name="no_note" value="1" />
            <input type="hidden" name="currency_code" value="EUR" />
            <input type="hidden" name="tax" value="0" />
            <input type="image" src="https://www.paypal.com/en_US/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="donate via PayPal" />
          </form>
        </div>
        
        <h1><a href="http://windstille.berlios.de/windstille/"><img border="0" src="images/windstille_small.png" alt="Windstille" /></a></h1>
        
        <table cellspacing="0" cellpadding="0" border="0" align="center" style="margin-bottom: 0em;">
          <tr>
            <td>
              <xsl:apply-templates select="document('menu.xml')" />
            </td>
          </tr>
        </table>

        
        <xsl:if test="$group != ''">
          <xsl:apply-templates select="document(concat($group, '-menu.xml'))" />
        </xsl:if>
        
        <xsl:apply-templates />
        
        <div class="copyright">
          Copyright &#0169; 2002 <a href="http://pingus.seul.org/~grumbel/">Ingo Ruhnke</a>, <a href="mailto:grumbel@gmx.de?subject=[Windstille] ">&lt;grumbel@gmx.de&gt;</a><br />
          Last update: <xsl:value-of select="$lastchange" /><br />
        </div>
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="section">
    <div class="section-box">
      <xsl:if test="@id!=''">
        <xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
      </xsl:if>
      <div class="section-title"><h2><xsl:value-of select="@title" /></h2></div>
      <div class="section-body">
        <xsl:apply-templates />
      </div>
    </div>
  </xsl:template>

  <xsl:template match="subsection">
    <h3><xsl:value-of select="@title" /></h3>
    <xsl:apply-templates />
    <br clear="all" />
  </xsl:template>

  <xsl:template match="subsubsection">
    <div style="padding-left: 1em; clear: both;">
      <h4><xsl:value-of select="@title" /></h4>
      <xsl:apply-templates />
    </div>
  </xsl:template>

  <xsl:template match="faq-list">
    <ul>
      <xsl:for-each select="faq/question">
        <li><a href="#faq{generate-id(.)}">
            <xsl:apply-templates/></a></li>
      </xsl:for-each>
    </ul>
    <hr/>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="faq">
    <p></p>
    <table width="100%"  class="question">
      <colgroup width="60%" />
      <tr><td valign="top">
          <div id="faq{generate-id(question)}">
            <xsl:apply-templates select="question/node()"/>
          </div>
        </td>
        
        
        <td align="right" valign="top">
          <small>Last update:<xsl:value-of select="@date"/></small>
          [<small><a href="#faqtoc">Up</a></small>]
        </td>
      </tr>
    </table>

    <p class="answer"><xsl:apply-templates select="answer/node()"/> </p>
  </xsl:template>
  
  <xsl:template match="news">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="news/item">
    <p style="padding: 0em;"><strong><xsl:value-of select="@date" /></strong> - <xsl:apply-templates /></p>
  </xsl:template>

  <!-- Menu Stuff -->
  <xsl:template match="menu">
    <div class="menu">
      <xsl:apply-templates />
    </div>
  </xsl:template>

  <xsl:template match="menu/item">
    <xsl:choose>
      <xsl:when test="concat($group, '.html')=@file or concat($filename, '.html')=@file">
        <a class="active" href="{@file}"><xsl:apply-templates /></a>
      </xsl:when>
      <xsl:otherwise>
        <a href="{@file}"><xsl:apply-templates /></a>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Submenu Stuff -->
  <xsl:template match="submenu">
    <div class="submenu">
      <table cellpadding="0" cellspacing="0" border="0" align="center">
        <tr>
          <xsl:apply-templates />
        </tr>
      </table>
    </div>
  </xsl:template>

  <xsl:template match="submenu/item">
    <td>
      <xsl:choose>
        <xsl:when test="concat($filename, '.html')=@file">
          <a class="active" href="{@file}"><xsl:apply-templates /></a>
        </xsl:when>
        <xsl:otherwise>
          <a href="{@file}"><xsl:apply-templates /></a>
        </xsl:otherwise>
      </xsl:choose>
    </td>
  </xsl:template>


</xsl:stylesheet>

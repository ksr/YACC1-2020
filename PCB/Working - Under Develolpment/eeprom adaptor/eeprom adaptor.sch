<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="atmel" urn="urn:adsk.eagle:library:105">
<description>&lt;b&gt;AVR Devices&lt;/b&gt;&lt;p&gt;
Configurable logic, microcontrollers, nonvolatile memories&lt;p&gt;
Based on the following sources:&lt;p&gt;
&lt;ul&gt;
&lt;li&gt;www.atmel.com
&lt;li&gt;CD-ROM : Configurable Logic Microcontroller Nonvolatile Memory
&lt;li&gt;CadSoft download site, www.cadsoft.de or www.cadsoftusa.com , file at90smcu_v400.zip
&lt;li&gt;avr.lbr
&lt;/ul&gt;
&lt;author&gt;Revised by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DIL08" urn="urn:adsk.eagle:footprint:4164/1" library_version="5">
<description>&lt;B&gt;Dual In Line&lt;/B&gt;</description>
<wire x1="-5.08" y1="-0.635" x2="-5.08" y2="0.635" width="0.1524" layer="21" curve="180"/>
<wire x1="-5.08" y1="-0.635" x2="-5.08" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-2.794" x2="5.08" y2="-2.794" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-2.794" x2="5.08" y2="2.794" width="0.1524" layer="21"/>
<wire x1="5.08" y1="2.794" x2="-5.08" y2="2.794" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="2.794" x2="-5.08" y2="0.635" width="0.1524" layer="21"/>
<pad name="1" x="-3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="1.27" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="3.81" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="-1.27" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="-3.81" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-5.3594" y="-2.54" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-3.81" y="-0.9906" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
</package>
</packages>
<packages3d>
<package3d name="DIL08" urn="urn:adsk.eagle:package:4324/1" type="box" library_version="5">
<description>Dual In Line</description>
<packageinstances>
<packageinstance name="DIL08"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="24CXX" urn="urn:adsk.eagle:symbol:4187/1" library_version="5">
<wire x1="-7.62" y1="10.16" x2="7.62" y2="10.16" width="0.254" layer="94"/>
<wire x1="7.62" y1="10.16" x2="7.62" y2="-10.16" width="0.254" layer="94"/>
<wire x1="7.62" y1="-10.16" x2="-7.62" y2="-10.16" width="0.254" layer="94"/>
<wire x1="-7.62" y1="-10.16" x2="-7.62" y2="10.16" width="0.254" layer="94"/>
<text x="-7.62" y="10.795" size="1.778" layer="95">&gt;NAME</text>
<text x="-7.62" y="-12.7" size="1.778" layer="96">&gt;VALUE</text>
<pin name="A2" x="10.16" y="7.62" length="short" direction="in" rot="R180"/>
<pin name="A1" x="10.16" y="5.08" length="short" direction="in" rot="R180"/>
<pin name="A0" x="10.16" y="2.54" length="short" direction="in" rot="R180"/>
<pin name="WP" x="10.16" y="-2.54" length="short" direction="in" rot="R180"/>
<pin name="SCL" x="10.16" y="-5.08" length="short" rot="R180"/>
<pin name="SDA" x="10.16" y="-7.62" length="short" rot="R180"/>
<pin name="GND" x="-10.16" y="-7.62" length="short" direction="pwr"/>
<pin name="VCC" x="-10.16" y="7.62" length="short" direction="pwr"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="AT24C*" urn="urn:adsk.eagle:component:4389/2" prefix="IC" library_version="5">
<description>2-wire serial &lt;B&gt;EEPROM&lt;/B&gt;</description>
<gates>
<gate name="G$1" symbol="24CXX" x="0" y="0"/>
</gates>
<devices>
<device name="P" package="DIL08">
<connects>
<connect gate="G$1" pin="A0" pad="1"/>
<connect gate="G$1" pin="A1" pad="2"/>
<connect gate="G$1" pin="A2" pad="3"/>
<connect gate="G$1" pin="GND" pad="4"/>
<connect gate="G$1" pin="SCL" pad="6"/>
<connect gate="G$1" pin="SDA" pad="5"/>
<connect gate="G$1" pin="VCC" pad="8"/>
<connect gate="G$1" pin="WP" pad="7"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:4324/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="AT24C1024BW-SH-B" constant="no"/>
<attribute name="OC_FARNELL" value="1362646" constant="no"/>
<attribute name="OC_NEWARK" value="03P2044" constant="no"/>
<attribute name="POPULARITY" value="5" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="IC1" library="atmel" library_urn="urn:adsk.eagle:library:105" deviceset="AT24C*" device="P" package3d_urn="urn:adsk.eagle:package:4324/1"/>
<part name="IC2" library="atmel" library_urn="urn:adsk.eagle:library:105" deviceset="AT24C*" device="P" package3d_urn="urn:adsk.eagle:package:4324/1"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="IC1" gate="G$1" x="50.8" y="73.66" smashed="yes">
<attribute name="NAME" x="43.18" y="84.455" size="1.778" layer="95"/>
<attribute name="VALUE" x="43.18" y="60.96" size="1.778" layer="96"/>
</instance>
<instance part="IC2" gate="G$1" x="50.8" y="38.1" smashed="yes">
<attribute name="NAME" x="43.18" y="48.895" size="1.778" layer="95"/>
<attribute name="VALUE" x="43.18" y="25.4" size="1.778" layer="96"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="VCC" class="0">
<segment>
<pinref part="IC2" gate="G$1" pin="A2"/>
<wire x1="60.96" y1="45.72" x2="66.04" y2="45.72" width="0.1524" layer="91"/>
<wire x1="66.04" y1="45.72" x2="66.04" y2="43.18" width="0.1524" layer="91"/>
<pinref part="IC2" gate="G$1" pin="A1"/>
<wire x1="66.04" y1="43.18" x2="60.96" y2="43.18" width="0.1524" layer="91"/>
<pinref part="IC2" gate="G$1" pin="VCC"/>
<wire x1="40.64" y1="45.72" x2="35.56" y2="45.72" width="0.1524" layer="91"/>
<wire x1="35.56" y1="45.72" x2="35.56" y2="53.34" width="0.1524" layer="91"/>
<wire x1="35.56" y1="53.34" x2="66.04" y2="53.34" width="0.1524" layer="91"/>
<wire x1="66.04" y1="53.34" x2="66.04" y2="45.72" width="0.1524" layer="91"/>
<junction x="66.04" y="45.72"/>
<pinref part="IC1" gate="G$1" pin="A2"/>
<wire x1="60.96" y1="81.28" x2="66.04" y2="81.28" width="0.1524" layer="91"/>
<wire x1="66.04" y1="81.28" x2="66.04" y2="78.74" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="A1"/>
<wire x1="66.04" y1="78.74" x2="60.96" y2="78.74" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="A0"/>
<wire x1="60.96" y1="76.2" x2="66.04" y2="76.2" width="0.1524" layer="91"/>
<wire x1="66.04" y1="76.2" x2="66.04" y2="78.74" width="0.1524" layer="91"/>
<junction x="66.04" y="78.74"/>
<pinref part="IC1" gate="G$1" pin="VCC"/>
<wire x1="40.64" y1="81.28" x2="35.56" y2="81.28" width="0.1524" layer="91"/>
<wire x1="35.56" y1="81.28" x2="35.56" y2="91.44" width="0.1524" layer="91"/>
<wire x1="35.56" y1="91.44" x2="66.04" y2="91.44" width="0.1524" layer="91"/>
<wire x1="66.04" y1="91.44" x2="66.04" y2="81.28" width="0.1524" layer="91"/>
<junction x="66.04" y="81.28"/>
<wire x1="35.56" y1="53.34" x2="35.56" y2="81.28" width="0.1524" layer="91"/>
<junction x="35.56" y="53.34"/>
<junction x="35.56" y="81.28"/>
<label x="38.1" y="91.44" size="1.778" layer="95"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="IC2" gate="G$1" pin="A0"/>
<wire x1="60.96" y1="40.64" x2="66.04" y2="40.64" width="0.1524" layer="91"/>
<wire x1="66.04" y1="40.64" x2="66.04" y2="22.86" width="0.1524" layer="91"/>
<wire x1="66.04" y1="22.86" x2="40.64" y2="22.86" width="0.1524" layer="91"/>
<pinref part="IC2" gate="G$1" pin="GND"/>
<wire x1="40.64" y1="22.86" x2="40.64" y2="30.48" width="0.1524" layer="91"/>
<wire x1="40.64" y1="30.48" x2="27.94" y2="30.48" width="0.1524" layer="91"/>
<junction x="40.64" y="30.48"/>
<pinref part="IC1" gate="G$1" pin="GND"/>
<wire x1="40.64" y1="66.04" x2="27.94" y2="66.04" width="0.1524" layer="91"/>
<wire x1="27.94" y1="66.04" x2="27.94" y2="30.48" width="0.1524" layer="91"/>
<junction x="40.64" y="66.04"/>
<label x="27.94" y="66.04" size="1.778" layer="95"/>
</segment>
</net>
<net name="SCL" class="0">
<segment>
<pinref part="IC2" gate="G$1" pin="SCL"/>
<wire x1="60.96" y1="33.02" x2="76.2" y2="33.02" width="0.1524" layer="91"/>
<label x="71.12" y="33.02" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="SCL"/>
<wire x1="60.96" y1="68.58" x2="76.2" y2="68.58" width="0.1524" layer="91"/>
<label x="71.12" y="68.58" size="1.778" layer="95"/>
</segment>
</net>
<net name="SDA" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="SDA"/>
<wire x1="60.96" y1="66.04" x2="76.2" y2="66.04" width="0.1524" layer="91"/>
<label x="71.12" y="66.04" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC2" gate="G$1" pin="SDA"/>
<wire x1="60.96" y1="30.48" x2="76.2" y2="30.48" width="0.1524" layer="91"/>
<label x="71.12" y="30.48" size="1.778" layer="95"/>
</segment>
</net>
<net name="WP" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="WP"/>
<wire x1="60.96" y1="71.12" x2="76.2" y2="71.12" width="0.1524" layer="91"/>
<label x="71.12" y="71.12" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC2" gate="G$1" pin="WP"/>
<wire x1="60.96" y1="35.56" x2="76.2" y2="35.56" width="0.1524" layer="91"/>
<label x="71.12" y="35.56" size="1.778" layer="95"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>

#include "metadatafile_generator.h"
#include<QDateTime>

QString MetaDataFileGenerator::generateRDF( const Process& proc,const Person& pers )
{
	QString txt("<rdf:RDF xml:lang=\"en\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns:rdfs=\"http://www.w3.org/2000/01/rdf-schema#\"  xmlns:crm= \"http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CIDOC-CRM.rdfs#\"  xmlns:crmdig=\"http://www.ics.forth.gr/isl/rdfs/3D-COFORM_CRMdig.rdfs#\">\n");
	txt += QString("<crmdig:D3.Formal_Derivation rdf:about=\"" + proc.uuidProc + "\">\n");
	txt += QString("<rdfs:label>" + proc.comment + "</rdfs:label>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#EventType/Process_Event\"></crm:E55.Type>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#EventType/Modeling_Process\"></crm:E55.Type>\n");
	txt += QString("</crm:P2F.has_type>\n\n");
	txt += QString("<crmdig:L31F.has_starting_date-time rdf:datatype=\"http://www.w3.org/2001/XMLSchema#dateTime\">\n");
	txt += QDateTime::currentDateTime().toString(Qt::ISODate) + "Z\n";
	txt += QString("</crmdig:L31F.has_starting_date-time>\n\n");
	txt += QString("<crmdig:L32F.has_ending_date-time rdf:datatype=\"http://www.w3.org/2001/XMLSchema#dateTime\">\n");
	txt += QDateTime::currentDateTime().toString(Qt::ISODate) + "Z\n";
	txt += QString("</crmdig:L32F.has_ending_date-time>\n\n");

	if (pers.org != NULL)
	{
		txt += QString("<crmdig:L29F.has_responsible_organization>\n");
		txt += QString("<crm:E40.Legal_Body rdf:about=\"" + pers.org->website + "\">\n");
		txt += QString("<rdfs:label>" + pers.org->name + "</rdfs:label>\n");
		txt += QString("<crm:P74F.has_current_or_former_residence>\n");
		txt += QString("<crm:E53.Place rdf:about=\"" + pers.org->geoRef + "\">\n");
		txt += QString("<rdfs:label>" + pers.org->city + "</rdfs:label>\n");
		txt += QString("<crm:P3F.has_note>" + pers.org->country + "</crm:P3F.has_note>\n");
		txt += QString("</crm:E53.Place>\n");
		txt += QString("</crm:P74F.has_current_or_former_residence>\n");
		txt += QString("</crm:E40.Legal_Body>\n");
		txt += QString("</crmdig:L29F.has_responsible_organization>\n");
	}

	txt += QString("<crmdig:L30F.has_operator>\n");
	txt += QString("<crm:E21.Person rdf:about=\"" + pers.uuid + "\">\n");
	txt += QString("<crm:P131F.is_identified_by>\n");
	txt += QString("<crmdig:D21.Person_Name>\n");
	txt += QString("<crmdig:L51F.has_first_name>" + pers.name + "</crmdig:L51F.has_first_name>\n");
	txt += QString("<crmdig:L52F.has_last_name>" + pers.surname + "</crmdig:L52F.has_last_name>\n");
	txt += QString("</crmdig:D21.Person_Name>\n");
	txt += QString("</crm:P131F.is_identified_by>\n");
	if (pers.org != NULL)
	{
		txt += QString("<crm:P107B.is_current_or_former_member_of>\n");
		txt += QString("<crm:E40.Legal_Body rdf:about=\"" + pers.org->website + "\"></crm:E40.Legal_Body>\n");
		txt += QString("</crm:P107B.is_current_or_former_member_of>\n");
	}
	txt += QString("</crm:E21.Person>\n");
	txt += QString("</crmdig:L30F.has_operator>\n\n");
	
	txt += QString("<crmdig:L2F.used_as_source>\n");
	txt += QString("<crmdig:D1.Digital_Object rdf:about=\"http://meshlab.sourceforge.com\">\n");
	txt += QString("<rdfs:label>" + proc.meshlabVer + "</rdfs:label>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#SoftwareType/3D-ModelSoftware\"></crm:E55.Type>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("</crmdig:D1.Digital_Object>\n");
	txt += QString("</crmdig:L2F.used_as_source>\n\n");

	if (proc.input.size() > 0)
	{
		txt += QString("<crmdig:L21F.used_as_derivation_source>\n");
		for(int ii = 0;ii < proc.input.size();++ii)
		{
			txt += QString("<crmdig:D9.Data_Object rdf:about=\"" + proc.input[ii].uuid + "\">\n");
			txt += QString("<rdfs:label>" + proc.input[ii].label + "</rdfs:label>\n");
			txt += QString("</crmdig:D9.Data_Object>\n");
		}
		txt += QString("</crmdig:L21F.used_as_derivation_source>\n\n");
	}

	txt += QString("<crmdig:L22F.created_derivative>\n");
	txt += QString("<crmdig:D9.Data_Object rdf:about=\"" + proc.output.uuid + "\">\n");
	txt += QString("<rdfs:label>" + proc.output.label + "</rdfs:label>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#ObjectType/mesh\"></crm:E55.Type>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#mimetype/ply\"></crm:E55.Type>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P43F.has_dimension>\n");
	txt += QString("<crm:E54.Dimension>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#ObjectType/Dimensions/Mesh\"/>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P90F.has_value rdf::datatype=\"http://www.w3.org/2001/XMLSchema#decimal\">" + QString::number(proc.output.nvt) + "</crm:P90F.has_value>\n");
	txt += QString("<crm:P91F.has_unit>\n");
	txt += QString("<crm:E58.Measurement_Unit rdf:about=\"http://www.3d-coform.eu/#ObjectType/Units/vertices\"/>\n");
	txt += QString("</crm:P91F.has_unit>\n");
	txt += QString("</crm:E54.Dimension>\n\n");

	txt += QString("<crm:E54.Dimension>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#ObjectType/Dimensions/Mesh\"/>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P90F.has_value rdf::datatype=\"http://www.w3.org/2001/XMLSchema#decimal\">" + QString::number(proc.output.nfc) + "</crm:P90F.has_value>\n");
	txt += QString("<crm:P91F.has_unit>\n");
	txt += QString("<crm:E58.Measurement_Unit rdf:about=\"http://www.3d-coform.eu/#ObjectType/Units/faces\"/>\n");
	txt += QString("</crm:P91F.has_unit>\n");
	txt += QString("</crm:E54.Dimension>\n\n");
	
	txt += QString("<crm:E54.Dimension>\n");
	txt += QString("<crm:P2F.has_type>\n");
	txt += QString("<crm:E55.Type rdf:about=\"http://www.3d-coform.eu/#ObjectType/Dimensions/DataWeight\"/>\n");
	txt += QString("</crm:P2F.has_type>\n");
	txt += QString("<crm:P90F.has_value rdf::datatype=\"http://www.w3.org/2001/XMLSchema#decimal\">" + QString::number(proc.output.megabyte) + "</crm:P90F.has_value>\n");
	txt += QString("<crm:P91F.has_unit>\n");
	txt += QString("<crm:E58.Measurement_Unit rdf:about=\"http://www.3d-coform.eu/#ObjectType/Units/Mb\"/>\n");
	txt += QString("</crm:P91F.has_unit>\n");
	txt += QString("</crm:E54.Dimension>\n\n");
	txt += QString("<crm:P43F.has_dimension>\n");
	txt += QString("<crm:P3F.has_note>\n");
	txt += QString("<![CDATA[" + proc.filtHistory + "]]>\n");
	txt += QString("</crm:P3F.has_note>\n");
	txt += QString("</crmdig:D9.Data_Object>\n");
	txt += QString("</crmdig:L22F.created_derivative>\n");
	txt += QString("</crmdig:D3.Formal_Derivation>\n");
	txt += QString("</rdf:RDF>\n");
	return txt;
}

MetaDataFileGenerator::Organization::Organization( const QString& nm,const QString& web,const QString& ct,const QString& cty,const QString& geo /*= QString()*/ )
:name(nm),website(web),city(ct),country(cty),geoRef(geo)
{
}

MetaDataFileGenerator::Person::Person( const QString& uuidPer,const QString& nm,const QString& srnm,Organization* orgz /*= NULL*/ )
:uuid(uuidPer),name(nm),surname(srnm),org(orgz)
{

}

MetaDataFileGenerator::MeshInfo::MeshInfo( const QString& lbl,const QString& uuidMesh,const int fcs,const int vert,const float mb )
:label(lbl),uuid(uuidMesh),nfc(fcs),nvt(vert),megabyte(mb)
{

}

MetaDataFileGenerator::Process::Process(const QString& comm,const QString& uuidPr,const QList<MeshInfo>& inp,const MeshInfo& out,const QString& mlver,const QString& filts )
:comment(comm),uuidProc(uuidPr),input(inp),output(out),meshlabVer(mlver),filtHistory(filts)
{

}
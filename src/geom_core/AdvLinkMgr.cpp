//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
//// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// AdvLinkMgr.h: Parm Adv Link Mgr Singleton.
//
//////////////////////////////////////////////////////////////////////

#include "AdvLinkMgr.h"
#include "AdvLink.h"
#include "FileUtil.h"
#include "ScriptMgr.h"
#include "ParmMgr.h"
#include "Vehicle.h"
#include "VehicleMgr.h"
#include "VSP_Geom_API.h"
#include "StringUtil.h"
#include "StlHelper.h"


//==== Constructor ====//
AdvLinkMgrSingleton::AdvLinkMgrSingleton()
{
    m_ActiveLink = NULL;
    m_EditLinkIndex = 0;

}

void AdvLinkMgrSingleton::Init()
{

}

void AdvLinkMgrSingleton::Wype()
{
    //==== Delete All Links ====//
    for ( int i = 0 ; i < (int)m_LinkVec.size() ; i++ )
    {
        if ( m_LinkVec[i] )
            delete m_LinkVec[i];
    }
    m_LinkVec.clear();
    m_ActiveLink = NULL;
    m_EditLinkIndex = 0;
}

void AdvLinkMgrSingleton::Renew()
{
    Wype();
    Init();
}

//==== Scan Custom Directory And Return All Possible Types ====//
void AdvLinkMgrSingleton::ReadAdvLinkScripts()
{
    //==== Only Read Once ====//
    static bool init_flag = false;
    if ( init_flag )
        return;
    init_flag = true;

    vector< string > mod_vec = ScriptMgr.ReadScriptsFromDir( "../../../LinkScripts/" );

}

//==== Check For Duplicate Link Name =====//
bool AdvLinkMgrSingleton::DuplicateLinkName( const string & name )
{
    for ( int i = 0 ; i < (int)m_LinkVec.size() ; i++ )
    {
        if ( m_LinkVec[i]->GetName() == name )
            return true;
    }
    return false;
}

//==== Add A Link =====//
AdvLink* AdvLinkMgrSingleton::AddLink( const string & name )
{
    //==== Create Unique Name ====//
    string base_name = name;
    if ( base_name.size() == 0 )
    {
        base_name = "Unnamed_Link";
    }
    string link_name = base_name;

    int cnt = 1;
    while ( DuplicateLinkName( link_name ) )
    {
        link_name = base_name + "_" + StringUtil::int_to_string( cnt, "%d" );
        cnt++;
    }

    AdvLink* alink = new AdvLink();
    alink->SetName( link_name );
    m_LinkVec.push_back( alink );
    m_EditLinkIndex = (int)m_LinkVec.size() - 1;

    return alink;
}

void AdvLinkMgrSingleton::DelLink( AdvLink* link_ptr )
{
    if ( !link_ptr )
        return;
    if ( m_ActiveLink == link_ptr )
        m_ActiveLink = NULL;
    m_EditLinkIndex = -1;

    vector_remove_val( m_LinkVec, link_ptr );
    delete link_ptr;
}

void AdvLinkMgrSingleton::DelAllLinks( )
{
    m_EditLinkIndex = -1;
    m_ActiveLink = NULL;

    for ( int i = 0 ; i < (int)m_LinkVec.size() ; i++ )
    {
        delete m_LinkVec[i];
    }
    m_LinkVec.clear();
}

void AdvLinkMgrSingleton::AddAdvLink( const string & script_module_name )
{
    //AdvLink* alink = new AdvLink();
    //alink->SetModuleName( script_module_name );
    //m_LinkVec.push_back( alink );
    //m_ActiveLink = alink;

    ////==== Call Script ====//
    //ScriptMgr.ExecuteScript( script_module_name.c_str(), "void AddVars()" );



}

AdvLink* AdvLinkMgrSingleton::GetLink( int index )
{
    if ( index >= 0 && index < (int)m_LinkVec.size() )
    {
        return m_LinkVec[index];
    }
    return NULL;

}

void AdvLinkMgrSingleton::AddInput( const string & parm_id, const string & var_name )
{
    AddInputOutput( parm_id, var_name, true );
}

void AdvLinkMgrSingleton::AddOutput( const string & parm_id, const string & var_name )
{
    AddInputOutput( parm_id, var_name, false );
}

void AdvLinkMgrSingleton:: AddInputOutput( const string & parm_id, const string & var_name, bool input_flag )
{
    AdvLink* edit_link = GetLink( GetEditLinkIndex() );
    if ( !edit_link )
        return;

    //==== Find Parm Ptr ===//
    Parm* parm_ptr = ParmMgr.FindParm( parm_id );
    if ( !parm_ptr )
        return;

    VarDef pd;
    pd.m_ParmID = parm_id;
    pd.m_VarName = var_name;

    edit_link->AddVar( pd, input_flag );
}


void AdvLinkMgrSingleton::SetVar( const string & var_name, double val )
{
    if ( !m_ActiveLink )
        return;

    m_ActiveLink->SetVar( var_name, val );

}

double AdvLinkMgrSingleton::GetVar( const string & var_name )
{
    if ( !m_ActiveLink )
        return 0.0;

    return m_ActiveLink->GetVar( var_name );
}


//==== Parm Changed ====//
void AdvLinkMgrSingleton::ParmChanged( const string& pid, bool start_flag  )
{
    //==== Find Parm Ptr ===//
    Parm* parm_ptr = ParmMgr.FindParm( pid );
    if ( !parm_ptr )
    {
        return;
    }

    //==== Check All Links And Update If Needed ====//
    bool updated_flag = false;
    for ( int i = 0 ; i < (int)m_LinkVec.size() ; i++ )
    {
        if ( m_LinkVec[i]->UpdateLink( pid ) )
            updated_flag = true;
    }

    if ( !updated_flag )
        return;

    //==== Reset Updated Flags ====//
    if ( start_flag )
    {
        //for ( int i = 0 ; i < ( int )m_UpdatedParmVec.size() ; i++ )
        //{
        //    Parm* parm_ptr = ParmMgr.FindParm( m_UpdatedParmVec[i] );
        //    if ( parm_ptr )
        //    {
        //        parm_ptr->SetLinkUpdateFlag( false );
        //    }
        //}
        //m_UpdatedParmVec.clear();

        Vehicle* veh = VehicleMgr.GetVehicle();
        if ( veh )
        {
            veh->ParmChanged( parm_ptr, Parm::SET );
        }
    }

}


xmlNodePtr AdvLinkMgrSingleton::EncodeXml( xmlNodePtr & node )
{
    xmlNodePtr linkmgr_node = xmlNewChild( node, NULL, BAD_CAST"AdvLinkMgr", NULL );

    for ( int i = 0 ; i < ( int )m_LinkVec.size() ; i++ )
    {
        if( m_LinkVec[i] )
        {
            m_LinkVec[i]->EncodeXml( linkmgr_node );
        }
    }


    return linkmgr_node;
}

xmlNodePtr AdvLinkMgrSingleton::DecodeXml( xmlNodePtr & node )
{
    xmlNodePtr linkmgr_node = XmlUtil::GetNode( node, "AdvLinkMgr", 0 );
    if ( linkmgr_node )
    {
        int num = XmlUtil::GetNumNames( linkmgr_node, "AdvLink" );
        for ( int i = 0 ; i < num ; i++ )
        {
            xmlNodePtr link_node = XmlUtil::GetNode( linkmgr_node, "AdvLink", i );
            if ( link_node )
            {
                AdvLink* link = AddLink( "" );
                link->DecodeXml( link_node );
                link->BuildScript();
            }
        }
    }

    return linkmgr_node;
}

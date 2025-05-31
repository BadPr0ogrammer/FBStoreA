#ifdef FBXSDK_SHARED
#include "fbxsdk.h"
using namespace fbxsdk;
#else
#include "ufbx.h"
#endif
#include "FbxLoader.h"

#include "App.h"
#include "MainWnd.h"
#include "LoginDlg.h"
#include "FBase.h"
#include "Utilities.h"

using namespace System::Windows::Data;
using namespace System::Windows::Markup;
using namespace System::IO;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Numerics;
using namespace Microsoft::Win32;

using namespace std;
using uchar = unsigned char;

namespace FBStoreA
{
	MainWnd::MainWnd()
	{
		FileStream^ fs = gcnew FileStream("MainWnd.xaml", FileMode::Open);
		_root = (DependencyObject^)XamlReader::Load(fs);
		fs->Close();

		Window^ tmp = (Window^)_root;
		WindowStartupLocation = tmp->WindowStartupLocation;
		Content = tmp->Content;
		DataContext = this;

		MenuItem^ file_mi = (MenuItem^)tmp->FindName("MenuItem_File");
		file_mi->FontSize = 20;

		MenuItem^ open_mi = (MenuItem^)tmp->FindName("MenuItem_Open");
		open_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnOpenFile);

		_login_mi = (MenuItem^)tmp->FindName("MenuItem_Login");
		_login_mi->FontSize = 20;
		_login_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnLogin);

		_user_mi = (MenuItem^)tmp->FindName("MenuItem_User");
		_user_mi->FontSize = 20;
		_user_mi->Click += gcnew RoutedEventHandler(this, &MainWnd::OnUser);

		_transform_sb_tb = (TextBlock^)tmp->FindName("TextBlock_Transform");

		BindMenuItem(_login_mi, "PropLogin");
		BindMenuItem(_user_mi, "PropUser");
		
		CommandBinding^ closeBinding = gcnew CommandBinding(ApplicationCommands::Close);
		closeBinding->Executed += gcnew ExecutedRoutedEventHandler(this, &MainWnd::OnClose);
		this->CommandBindings->Add(closeBinding);

		_modelGroup = gcnew Model3DGroup();
		_modelVisual = gcnew ModelVisual3D();

		Grid^ grid = (Grid^)tmp->FindName("Grid_Viewport");
		_viewport = gcnew HelixViewport3D();
		grid->Children->Add(_viewport);

		_viewport->ShowFrameRate = true;
		_viewport->ShowCameraInfo = true;
		_viewport->ShowCoordinateSystem = true;
		_viewport->ShowTriangleCountInfo = true;
		_viewport->ZoomExtentsWhenLoaded = false;
		_viewport->ZoomAroundMouseDownPoint = true;
		_viewport->RotateAroundMouseDownPoint = true;
		_viewport->IsTopBottomViewOrientedToFrontBack = true;
		_viewport->IsViewCubeEdgeClicksEnabled = true;
		_viewport->Children->Add(gcnew SunLight());

		_viewport->Children->Add(_modelVisual);
		_viewport->MouseMove += gcnew MouseEventHandler(this, &MainWnd::OnViewportMouseMove);

		_modelVisual->Content = _modelGroup;
	}

	void MainWnd::OnLogin(Object^ sender, RoutedEventArgs^ e)
	{
		App^ app = (App^)Application::Current;
		if (app->_fbptr->IsLogin())
		{
			app->_fbptr->Logout();
			PropLogin = "Login";
			PropUser = "";
		}
		else
		{
			LoginDlg^ loginDlg = gcnew LoginDlg();
			auto result = loginDlg->ShowDialog();
			if (result)
			{
				if (loginDlg->_anon)
				{
					if (app->_fbptr->LoginAnon())
					{
						PropLogin = "Logout";
						PropUser = "<Anon.>";
						MessageBox::Show("Login Anonymously!", "FBStoreA");
					}
				} else if (loginDlg->_tokenEndpoint != nullptr) {
					std::string access_token = ConvertToStdString(loginDlg->_tokenEndpoint["access_token"]);
					std::string id_token = ConvertToStdString(loginDlg->_tokenEndpoint["id_token"]);
					if (app->_fbptr->Login(id_token, access_token))
					{
						PropLogin = "Logout";
						PropUser = gcnew String(app->_fbptr->_user.c_str());
						std::string msg = "Login " + app->_fbptr->_user + " !";
						MessageBox::Show(gcnew String(msg.c_str()), "FBStoreA");
					}
				}
			}
		}
	}

	void MainWnd::OnClose(Object^ sender, ExecutedRoutedEventArgs^ e) 
	{
		App^ app = (App^)Application::Current;
		delete app->_fbptr;
		this->Close(); 

		delete _fbxLoader;
		_fbxLoader = nullptr;

		/////////////_modelVisual->Clear()
		//_modelGroup = nullptr;

	}

	void MainWnd::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	void MainWnd::BindMenuItem(MenuItem^ menuItem, String^ propertyName)
	{
		Binding^ binding = gcnew Binding(propertyName);
		binding->Source = this;
		menuItem->SetBinding(MenuItem::HeaderProperty, binding);
	}
#ifdef FBXSDK_SHARED
	void MainWnd::BuildMeshes(FbxNode* node)
	{
		if (!node)
			return;
		int geometryCount = node->GetSrcObjectCount<FbxGeometry>();
		for (int i = 0; i < geometryCount; i++)
		{
			FbxGeometry* geometry = node->GetSrcObject<FbxGeometry>(i);
			if (geometry && geometry->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = geometry->GetNode()->GetMesh();
				auto meshBuilder = gcnew MeshBuilder(false, false, false);

				auto greenMt = MaterialHelper::CreateMaterial(Colors::Green);

				FbxLayer* layer = mesh->GetLayer(0);
				FbxLayerElementMaterial* pMaterialLayer = layer->GetMaterials();
				int numMatIndices = pMaterialLayer->GetIndexArray().GetCount();
				for (int k = 0; k < numMatIndices; k++)
				{
					int matIndex = pMaterialLayer->GetIndexArray()[k];
					FbxSurfaceMaterial* smat = mesh->GetNode()->GetMaterial(matIndex);
					if (smat->GetClassId().Is(FbxSurfaceLambert::ClassId))
					{
						FbxSurfaceLambert* surf = (FbxSurfaceLambert*)smat;
						FbxPropertyT<FbxDouble3> p = surf->Emissive;
						FbxDouble3 color = p.Get();
						greenMt = MaterialHelper::CreateMaterial(Color::FromRgb((uchar)(255 * color[0]), (uchar)(255 * color[1]), (uchar)(255 * color[2])));
						break;
					}
				}

				int polygonCount = mesh->GetPolygonCount();
				FbxVector4* controlPoints = mesh->GetControlPoints();
				for (int j = 0; j < polygonCount; j++)
				{
					int vertexCount = mesh->GetPolygonSize(j);
					auto points = gcnew List<Vector3>();
					for (int k = 0; k < vertexCount; k++)
					{
						int vertexIndex = mesh->GetPolygonVertex(j, k);
						FbxVector4 vertex = controlPoints[vertexIndex];
						points->Add(Vector3((float)vertex[0], (float)vertex[1], (float)vertex[2]));
					}
					meshBuilder->AddPolygon(points);
				}
				auto gm = gcnew GeometryModel3D(ConverterExtensions::ToWndMeshGeometry3D(meshBuilder->ToMesh(), true), greenMt);
				try
				{
					const auto& tr = node->LclTranslation.Get();
					const auto& rt = node->LclRotation.Get();
					const auto& sc = node->LclScaling.Get();
					
					FbxAMatrix trM;
					trM.SetS(tr);
					FbxAMatrix rtM;
					rtM.SetS(rt);
					FbxAMatrix scM;
					scM.SetS(sc);
					FbxAMatrix all = trM * rtM * scM;
					
					auto m = gcnew Matrix3D();
					m->M11 = all[0][0];
					m->M12 = all[0][1];
					m->M13 = all[0][2];
					m->M14 = all[0][3];
					m->M21 = all[1][0];
					m->M22 = all[1][1];
					m->M23 = all[1][2];
					m->M24 = all[1][3];
					m->M31 = all[2][0];
					m->M32 = all[2][1];
					m->M33 = all[2][2];
					m->M34 = all[2][3];
					m->M44 = all[3][3];
					gm->Transform = gcnew MatrixTransform3D(*m);
				}
				catch (...)
				{
				}

				_modelGroup->Children->Add(gm);
			}
		}
		for (int i = 0; i < node->GetChildCount(); i++)
			BuildMeshes(node->GetChild(i));
	}
#else
	void MainWnd::BuildMeshes(ufbx_node* node)
	{
		if (!node)
			return;
		ufbx_mesh* mesh = node->mesh;
		if (mesh)
		{
			auto meshBuilder = gcnew MeshBuilder(false, false, false);
			auto greenMt = MaterialHelper::CreateMaterial(Colors::Green);
								
			const ufbx_face* faces = mesh->faces.data;
			for (int j = 0; j < mesh->faces.count; j++)
			{
				auto points = gcnew List<Vector3>();
				const ufbx_face& face = faces[j];
				for (int k = 0; k < face.num_indices; k++)
				{
					ufbx_vec3 vertex = ufbx_get_vertex_vec3(&mesh->vertex_position, face.index_begin + k);
					points->Add(Vector3((float)vertex.x, (float)vertex.y, (float)vertex.z));
				}
				meshBuilder->AddPolygon(points);
			}
			auto gm = gcnew GeometryModel3D(ConverterExtensions::ToWndMeshGeometry3D(meshBuilder->ToMesh(), true), greenMt);
			_modelGroup->Children->Add(gm);
		}
		for (int i = 0; i < node->children.count; i++)
			BuildMeshes(node->children[i]);
	}
#endif

	MainWnd::~MainWnd()
	{
		delete _fbxLoader;
	}

	void MainWnd::OnOpenFile(Object^ sender, RoutedEventArgs^ e)
	{
		OpenFileDialog^ dlg = gcnew OpenFileDialog();
		if (dlg->ShowDialog())
		{
			delete _fbxLoader;
			_fbxLoader = new FbxLoader();

			string fname = ConvertToStdString(dlg->FileName);
			_fbxLoader->importFile(fname.c_str());
	
#ifdef FBXSDK_SHARED
			BuildMeshes((FbxNode*)(_fbxLoader->_scene));
#else
			BuildMeshes(_fbxLoader->_scene->root_node);
#endif
		}
	}

	void MainWnd::OnUser(Object^ sender, RoutedEventArgs^ e)
	{
	}

	void MainWnd::OnViewportMouseMove(Object^ sender, MouseEventArgs^ e)
	{
		if (e->RightButton == MouseButtonState::Pressed
			|| e->MiddleButton == MouseButtonState::Pressed)
		{
			auto cam = _viewport->Camera;
			Matrix3D viewMx = CameraHelper::GetViewMatrix(cam);
			Matrix3D projMx = CameraHelper::GetProjectionMatrix(cam, _viewport->ActualWidth / _viewport->ActualHeight);
			Matrix3D viewProjMx = viewMx * projMx;
			String^ str = String::Format(
				"{0:F2},{1:F2},{2:F2},{3:F2},{4:F2},{5:F2},{6:F2},{7:F2},{8:F2},{9:F2},{10:F2},{11:F2},{12:F2}",
				viewProjMx.M11, viewProjMx.M12, viewProjMx.M13, viewProjMx.M14,
				viewProjMx.M21, viewProjMx.M22, viewProjMx.M23, viewProjMx.M24,
				viewProjMx.M31, viewProjMx.M32, viewProjMx.M33, viewProjMx.M34,
				viewProjMx.M44);
			_transform_sb_tb->Text = str;
		}
	}
}

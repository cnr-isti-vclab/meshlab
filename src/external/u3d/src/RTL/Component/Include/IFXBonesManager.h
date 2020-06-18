//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
   @file IFXBonesManager.h
*/

#ifndef IFXBONESMANAGER_H
#define IFXBONESMANAGER_H

class IFXMeshGroup;
class IFXMeshGroup_Character;
class IFXMeshFactory;
class IFXVertexMapGroup;
class IFXMotionMixer;
class IFXCharacter;
class IFXTransform;
class IFXBlendParam;


/// Accepts text string describing progress.
typedef void (*IFXProgressCB)(const char *pText);

/// Returns precise relative timer value in milliseconds.
typedef float (*IFXGetTimerCB)(void);


/**
	An interface to IFXBonesManager_Impl, representing a subset of the 
	bones-based animation.

	This is an (almost) headerless interface to IFXBonesManager_Impl,
	representing a subset of the bones-based animation.

	You should not delete a IFXBonesManager directly.
	It may seem like it works, but destructors will not be called
	and you will leak memory.  You must use ifxDestroyBonesManager().

	@note	You cannot instantiate a IFXBonesManager directly.
	You must use IFXCreateBonesManager() to create an IFXBonesManager_Impl
	and return it as a IFXBonesManager.

	@warning Although you can send a new mesh by just using UpdateMesh()
		the old vertexmap must still be valid for that mesh.

	LoadMesh() is an alternative to InitMesh() for stand-alone tests.
*/
class IFXBonesManager
{
public:
	virtual ~IFXBonesManager() {}
	enum BooleanProperty
	{
		// effective during run time
		ShowBones,			///< display bones as wireframe
		ShowBoneNames,		///< display bone name text
		ShowImplants,		///< display bone implants
		ShowGrid,			///< display world grid
		ShowWorldAxes,		///< display axes at world origin
		ShowAxes,			///< display axes on every bone
		ShowJoints,			///< display joint cross-sections
		ShowCylinders,		///< display cross-sections connected
		ShowConstraints,	///< display IK constraint arcs
		ShowWeights,		///< display weights on selected bone
		ShowAllWeights,		///< display all weights with RGB blend
		ShowEffectors,		///< display IK effectors
		ShowBoneLinks,		///< display bone links
		ShowVertices,		///< display vertex text (slow)
		ShowNormals,		///< display normals as red lines with yellow,
							///< showing diff from length of 1.0
		AntiAliasLine,		///< toggle line anti-aliasing
		ApplyIK,			///< toggle Inverse Kinematics
		LimitAngVelocity,	///< limit IK changes
		ReferenceLock,		///< lock bones to reference position
		RootLock,			///< lock root bone(s) to reference location
		RootClearTranslate,	///< remove root bone(s) translation
		RootClearRotate,	///< remove root bone(s) rotation
		NoDeform,			///< only compute bones, do not deform
		Renormalize,		///< renormalize normals after deformation
		IKIncremental,		///< apply IK incrementally
		TimerUpdate,		///< sets itself when new timer data,
							///< clears itself when read (read-only)
		CLODLimited,		///< only deform vertices under CLOD limit
		AutoTranslationX,	///< chained anims attach head to tail
		AutoTranslationY,
		AutoTranslationZ,
		AutoRotationX,
		AutoRotationY,
		AutoRotationZ,
		AutoScaleX,
		AutoScaleY,
		AutoScaleZ,
		AutoBlend,			///< use auto blendtime vs. BlendFraction

		// only effective at load time
		LoadJustWeights,	///< does not reload bones reference data
		CreateBoneLinks,	///< use bone links
		RegenerateWeights,	///< create new weights algorithmically
		RemoveRogueWeights,	///< remove spurious patches of weights
		SmoothWeights,		///< smooth transitions in weights
		SmoothWeld,			///< smooth close vertices collectively
		PrepareForSave,		///< store info allowing bone save

		// changed per bone using IntegerProperty SelectBone
		AutoJoint,			///< automatically compute joint section
		AutoLink,			///< automatically compute link params
		ForTip,				///< apply joint edits to bone tip vs base
		IsInfluential,		///< bone is used in weight regeneration
		MaxBoolean };

	enum IntegerProperty
	{
		// effective during run time
		SelectBone,			///< pick specific bone by id
		NumberOfBones,		///< number of bones to pick from (read-only)
		DefaultBoneLinks,	///< default number of links for AutoLink
		ShowSingleMesh,		///< display data for a single mesh (meshid+1)
		VertexCycle,		///< only display vertex data where 
		///< vertexid%VertexCycles=VertexCycle
		VertexCycles,
		IKIterations,		///< number of iteration per frame for IK

		// changed per bone using IntegerProperty SelectBone
		NumBoneLinks,		///< number of links if AutoLink not set
		MaxInteger };

	// effective during run time
	enum FloatProperty
	{
		Time,				///< current animation time
		MinTime,			///< min time of current motion (read-only)
		MaxTime,			///< max time of current motion (read-only)
		BoneWidthScale,		///< scaling ratio for bones display
		BoneMaxWidth,		///< scaling limit for bones
		GridSize,			///< size of world grid
		AxisLength,			///< length of bone axis for display
		WorldAxisLength,	///< length of world axis for display
		MaxAngVelocity,		///< max angular velocity for IK
		BlendFraction,		///< fraction from old to new mixer
		BlendStartTime,		///< time which blend begins
		///< reset by BlendToMixer()
		ReachingLimit,		///< max distance of any vertex from root
		///< (with fixed bone length and displacement)

		// only effective at load time
		ModelSize,			///< approximate size of model
		SmoothThreshold,	///< max delta for smoothing
		WeldMax,			///< max distance to smooth collectively

		// changed per bone using IntegerProperty SelectBone
		LinkLength,			///< total length of bone links
		JointCenterY,		///< joint cross-section center and radials
		JointCenterZ,		///< (dependent on BooleanProperty ForTip)
		JointScaleY,
		JointScaleZ,
		MaxFloat };

	/// for built-in run-time profiling, subject to change
	enum TimedItem {
		TimeFrame,			///< frame to next frame
		TimeRenderInfo,		///< lines/points/text etc.
		TimeOutside,		///< everything but animation
		TimeAllAnim,		///< everything in animation
		TimeMotion,			///< applying motion to bones
		TimeLastMotion,		///< applying last motion to bones
		TimeBlend,			///< motion blending
		TimeIK,				///< applying IK to bones
		TimeLinkRecalc,		///< updating links per frame
		TimeBoneCache,		///< building bone transform cache
		TimeDeformation,	///< apply core vertex blending
		TimeRenormalize,	///< renormalizing normals
		NumTimedItems };


	/// Sets the reference matrix of each bone to the current
	/// bone position/rotation
	virtual IFXRESULT IFXAPI	UpdateReferencePositions() =0;
	
	/// Sets a boolean property
	virtual	IFXRESULT IFXAPI 	SetBool(BooleanProperty prop, bool set) =0;
	/// Gets a boolean property
	virtual	IFXRESULT IFXAPI 	GetBool(BooleanProperty prop, bool *pGet) =0;
	/// Sets an integer property
	virtual	IFXRESULT IFXAPI 	SetInteger(IntegerProperty prop, I32 set) =0;
	/// Gets an integer property
	virtual	IFXRESULT IFXAPI 	GetInteger(IntegerProperty prop,I32 *pGet) =0;
	/// Sets a real property
	virtual	IFXRESULT IFXAPI 	SetFloat(FloatProperty prop, F32 set) =0;
	/// Gets a real property
	virtual	IFXRESULT IFXAPI 	GetFloat(FloatProperty prop, F32 *pGet) =0;
	
	/// Setup mesh (usually called only once).
	virtual	IFXRESULT IFXAPI 	InitMesh(IFXMeshGroup *pMeshGrp,
										 IFXVertexMapGroup *pVertexMapGroup) =0;
	
	/// Applies animation to a mesh (for every frame)
	/// (same pVertexMapGroup assumed).
	virtual IFXRESULT IFXAPI UpdateMesh(
		IFXMeshGroup *pIn, IFXMeshGroup **ppOut) =0;
	
	// `extended interface'

	/** Deforms the mesh. Useful when DeformMesh bool is turned off
	to skip deformation in UpdateMesh(). */
	virtual	IFXRESULT IFXAPI DeformMesh(IFXMeshGroup *pIn,
										IFXMeshGroup **ppOut)=0;

	/// Resets manager as though just instantiated.
	virtual	IFXRESULT IFXAPI Reset(void) =0;
	/// Resets mixer assignment and autotranslation.
	virtual	IFXRESULT IFXAPI ResetUnshared(void) =0;
	/// Tell bones manager how to create a mesh.
	virtual	IFXRESULT IFXAPI SetMeshFactory(IFXMeshFactory *pSet)			=0;
	/// Prepares existing bones data.
	virtual	IFXRESULT IFXAPI PrepareBones(void)								=0;
	/// Looks up bone index by name.
	virtual	IFXRESULT IFXAPI GetBoneIndex(IFXString bonename,I32 *pBoneId)	=0;

	/** Looks up bone name by index
		@return	An IFXRESULT value
		@retval	IFX_OK	No error
		@retval	IFX_E_INVALID_POINTER 2nd input arguments was NULL.
	 */
	virtual	IFXRESULT IFXAPI GetBoneName(I32 boneId, IFXString *pBoneName)=0;

	/// Quickly switches to an existing motion mixer.
	virtual	IFXRESULT IFXAPI SelectMotionMixer(IFXMotionMixer *pSelect)		=0;
	
	/// Blends from one mixer to the next.
	virtual	IFXRESULT IFXAPI BlendToMotionMixer(
		IFXMotionMixer *pNext,
		const IFXBlendParam *pBlendParam) =0;

	/// Sets up Inverse Kinematics chain between two bones.
	virtual	IFXRESULT IFXAPI CreateIKChain(I32 baseBoneId, I32 endBoneId) =0;

	/** Gets data removed by RootClearTranslate and/or RootClearRotate */
	virtual	IFXRESULT IFXAPI GetRootClearance(IFXTransform *pTransform) =0;

	/// Sets current vector for auto-translation.
	virtual	IFXRESULT IFXAPI SetAutoTranslate(const IFXVector3& rVector,
											  bool last)=0;

	/// Sets current quaternion for auto-rotation.
	virtual	IFXRESULT IFXAPI SetAutoRotate(const IFXQuaternion& rQuaternion,
										   bool last) =0;
	
	/// Sets current vector for auto-scale.
	virtual	IFXRESULT IFXAPI SetAutoScale(const IFXVector3& rVector, 
										  bool last)=0;
	
	/// Alters current vector for auto-translation.
	virtual	IFXRESULT IFXAPI AddToAutoTranslate(const IFXVector3& rAddVector,
											bool last) =0;

	/// Alters current quaternion for auto-rotation.
	virtual	IFXRESULT IFXAPI AddToAutoRotate(
		const IFXQuaternion &rAddQuaternion,
		bool last)	=0;

	/// Alters current vector for auto-scale.
	virtual	IFXRESULT IFXAPI AddToAutoScale(const IFXVector3 &rAddVector,
											bool last) =0;

	virtual IFXRESULT IFXAPI GetMotionMixer(U32 index,
											IFXMotionMixer **ppMotionMixer)=0;

	// `test interface'
	
	/// Gets hidden instance of (IFXCharacter *).
	/// @note You must be able to declare an (IFXCharacter *) to use.
	/// This requires lots of extra headers.
	virtual	IFXRESULT IFXAPI GetCharacter(IFXCharacter **ppCharacter) =0;

	/// Provides a function that is to be given (char *).
	/// Provides a message explaining what the bones manager is doing.
	virtual	IFXRESULT IFXAPI SetProgressCallback(IFXProgressCB progressCB) =0;

	/// Provides a function that returns a float providing
	/// a precise incrementing fractional millisecond counter
	/// (required for timing operations).
	virtual	IFXRESULT IFXAPI SetGetTimerCallback(IFXGetTimerCB getTimerCB) =0;

	/// Finds time spent in specific timed area.
	virtual	IFXRESULT IFXAPI GetTimedPeriod(TimedItem prop,F32 *pGet) =0;

	/// Replaces auto-allocated character with another.
	virtual	void IFXAPI ReplaceCharacter(IFXMeshGroup_Character *pSet) =0;
};

/// @todo remove this code before Gold
//Short text name of timed areas
//const char	IFXBonesManagerTimedItemName
//								[IFXBonesManager::NumTimedItems][16]=
//			{ "Frame", "Info", "Out",
//				"Anim", "Mot", "Last", "Blend",
//				"IK", "Link",
//				"Cache", "Deform", "Norm" };

#endif

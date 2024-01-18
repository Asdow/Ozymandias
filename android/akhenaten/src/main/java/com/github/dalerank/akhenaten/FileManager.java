package com.github.dalerank.akhenaten;

import android.app.Activity;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;

import java.io.File;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;

public class FileManager {
    private static Uri baseUri = Uri.EMPTY;
    private static final HashMap<Uri, HashMap<String, FileInfo>> directoryStructureCache = new HashMap<>();

    private static final int FILE_TYPE_DIR = 1;
    private static final int FILE_TYPE_FILE = 2;

    private static String decodeUrl(String encodedUrl) {
        StringBuilder decodedUrlBuilder = new StringBuilder();
        char[] charArray = encodedUrl.toCharArray();

        for (int i = 0; i < charArray.length; i++) {
            char currentChar = charArray[i];

            if (currentChar == '%' && i + 2 < charArray.length) {
                // Check if there are two characters following '%' for a valid encoded sequence
                char hex1 = charArray[i + 1];
                char hex2 = charArray[i + 2];

                try {
                    // Convert the two hexadecimal characters to a decimal value and append the corresponding character
                    int decimalValue = Integer.parseInt("" + hex1 + hex2, 16);
                    decodedUrlBuilder.append((char) decimalValue);
                    i += 2; // Skip the two characters processed
                } catch (NumberFormatException e) {
                    // Ignore invalid encoded sequence
                    decodedUrlBuilder.append(currentChar);
                }
            } else {
                // Append the character as it is
                decodedUrlBuilder.append(currentChar);
            }
        }

        return decodedUrlBuilder.toString();
    }

    @SuppressWarnings("unused")
    public static String getPharaohPath(AkhenatenMainActivity activity) {
        String decodedPath = decodeUrl(baseUri.toString());
        baseUri = Uri.parse(decodedPath);

        //try {
        //    String[] split = baseUri.toString().split(":");
        //    String absPath = Environment.getExternalStorageDirectory().toString();
        //    String absDirName = absPath + File.separator + split[2];
        //    return absDirName;
        //}  catch (Exception e) {
        //    Log.e("akhenaten", "Error in getPharaohPath: " + e);
        //}

        return baseUri.toString();
    }

    @SuppressWarnings("unused")
    public static int setBaseUri(String path) {
        directoryStructureCache.clear();
        return setBaseUri(Uri.parse(path));
    }

    static void clearCache() {
        directoryStructureCache.clear();
    }

    static int setBaseUri(Uri newUri) {
        try {
            baseUri = newUri;
            FileInfo.base = new FileInfo(DocumentsContract.getTreeDocumentId(newUri), null,
                    DocumentsContract.Document.MIME_TYPE_DIR, 0, Uri.EMPTY);
            return 1;
        } catch (Exception e) {
            Log.e("akhenaten", "Error in setBaseUri: " + e);
            return 0;
        }
    }

    private static HashMap<String, FileInfo> getDirectoryContents(Activity activity, FileInfo dir) {
        if (!dir.isDirectory()) {
            return new HashMap<>();
        }
        return getDirectoryContents(activity, dir.getUri());
    }

    private static HashMap<String, FileInfo> getDirectoryContents(Activity activity, Uri dir) {
        HashMap<String, FileInfo> result = directoryStructureCache.get(dir);
        if (result != null) {
            return result;
        }
        result = new HashMap<>();
        Uri children = DocumentsContract.buildChildDocumentsUriUsingTree(dir, DocumentsContract.getDocumentId(dir));
        String[] columns = new String[] {
                DocumentsContract.Document.COLUMN_DOCUMENT_ID,
                DocumentsContract.Document.COLUMN_DISPLAY_NAME,
                DocumentsContract.Document.COLUMN_MIME_TYPE,
                DocumentsContract.Document.COLUMN_LAST_MODIFIED
        };
        Cursor cursor = activity.getContentResolver().query(children, columns, null, null, null);
        if (cursor != null) {
            while (cursor.moveToNext()) {
                FileInfo fileInfo = new FileInfo(cursor.getString(0), cursor.getString(1), cursor.getString(2),
                        cursor.getLong(3), dir);
                result.put(cursor.getString(1).toLowerCase(), fileInfo);
            }
            cursor.close();
        }
        directoryStructureCache.put(dir, result);
        return result;
    }

    private static FileInfo findFile(Activity activity, FileInfo folder, String fileName) {
        return getDirectoryContents(activity, folder).get(fileName.toLowerCase());
    }

    private static FileInfo getDirectoryFromPath(Activity activity, String[] path) {
        FileInfo currentDir = FileInfo.base;

        for (int i = 0; i < path.length - 1; ++i) {
            currentDir = findFile(activity, currentDir, path[i]);
            if (currentDir == null || !currentDir.isDirectory()) {
                return null;
            }
        }
        return currentDir;
    }

    private static FileInfo getFileFromPath(AkhenatenMainActivity activity, String filePath) {
        try {
            if (baseUri == Uri.EMPTY) {
                return null;
            }
            String[] filePart = filePath.split("(\\|/)");
            FileInfo dirInfo = getDirectoryFromPath(activity, filePart);
            if (dirInfo == null) {
                return null;
            }
            return findFile(activity, dirInfo, filePart[filePart.length - 1]);
        } catch (Exception e) {
            Log.e("akhenaten", "Error in getFileFromPath: " + e);
            return null;
        }
    }

    @SuppressWarnings("unused")
    public static FileInfo[] getDirectoryFileList(AkhenatenMainActivity activity, String dir, int type, String ext) {
        ArrayList<FileInfo> fileList = new ArrayList<>();

        if (baseUri == Uri.EMPTY) {
            return new FileInfo[0];
        }
        FileInfo lookupDir = FileInfo.base;
        if (!dir.equals(".")) {
            dir += "/.";
            lookupDir = getDirectoryFromPath(activity, dir.split("[\\\\/]"));
            if (lookupDir == null) {
                return new FileInfo[0];
            }
        }
        for (FileInfo file : getDirectoryContents(activity, lookupDir).values()) {
            if (((type & FILE_TYPE_FILE) == 0 && !file.isDirectory()) ||
                    ((type & FILE_TYPE_DIR) == 0 && file.isDirectory())) {
                continue;
            }
            if (!file.isDirectory() && !ext.isEmpty()) {
                String fileName = file.getName();
                int extCharPos = fileName.lastIndexOf('.');
                String currentExtension = fileName.substring(extCharPos + 1);
                if (currentExtension.equalsIgnoreCase(ext)) {
                    fileList.add(file);
                }
            } else {
                fileList.add(file);
            }
        }
        FileInfo[] result = new FileInfo[fileList.size()];
        return fileList.toArray(result);
    }

    public static boolean deleteFile(AkhenatenMainActivity activity, String filePath) {
        try {
            FileInfo fileInfo = getFileFromPath(activity, filePath);
            if (fileInfo == null) {
                return false;
            }
            HashMap<String, FileInfo> dirList = directoryStructureCache.get(fileInfo.getParentUri());
            if (dirList != null) {
                dirList.remove(fileInfo.getName().toLowerCase());
            }
            return DocumentsContract.deleteDocument(activity.getContentResolver(), fileInfo.getUri());
        } catch (Exception e) {
            Log.e("akhenaten", "Error in deleteFile: " + e);
            return false;
        }
    }

    @SuppressWarnings("unused")
    public static int openFileDescriptor(AkhenatenMainActivity activity, String filePath, String mode) {
        try {
            if (baseUri == Uri.EMPTY) {
                return 0;
            }
            if (filePath.startsWith("./")) {
                filePath = filePath.substring(2);
            }
            String internalMode = "";
            boolean isWrite = false;
            // Either "r" or "w"
            if (mode.indexOf('r') != -1) {
                internalMode += "r";
            }
            if (mode.indexOf('w') != -1) {
                internalMode += "w";
                isWrite = true;
            }

            String[] fileParts = filePath.split("[\\\\/]");
            String fileName = fileParts[fileParts.length - 1];
            FileInfo folderInfo = getDirectoryFromPath(activity, fileParts);
            if (folderInfo == null) {
                return 0;
            }
            FileInfo fileInfo = findFile(activity, folderInfo, fileName);
            Uri fileUri;
            if (fileInfo == null) {
                if (!isWrite) {
                    return 0;
                }
                fileUri = DocumentsContract.createDocument(activity.getContentResolver(),
                        folderInfo.getUri(), "application/octet-stream", fileName);
                if (fileUri == null) {
                    return 0;
                }
                HashMap<String, FileInfo> dirCache = directoryStructureCache.get(folderInfo.getUri());
                if (dirCache != null) {
                    fileInfo = new FileInfo(DocumentsContract.getDocumentId(fileUri),
                            fileName, "application/octet-stream", System.currentTimeMillis(), folderInfo.getUri());
                    dirCache.put(fileName.toLowerCase(), fileInfo);
                }
            } else {
                fileUri = fileInfo.getUri();
                if (isWrite) {
                    fileInfo.updateModifiedTime();
                }
            }
            ParcelFileDescriptor pfd = activity.getContentResolver().openFileDescriptor(fileUri, internalMode);
            return (pfd == null) ? 0 : pfd.detachFd();
        } catch (Exception e) {
            Log.e("akhenaten", "Error in openFileDescriptor: " + e);
            return 0;
        }
    }

    @SuppressWarnings("unused")
    public static class FileInfo {
        static FileInfo base;
        private final String documentId;
        private final String name;
        private final String mimeType;
        private final Uri parent;
        private long modifiedTime;
        private Uri uri;

        private FileInfo(String documentId, String name, String mimeType, long modifiedTime, Uri parent) {
            this.documentId = documentId;
            this.name = name;
            this.mimeType = mimeType;
            this.parent = parent;
            this.modifiedTime = modifiedTime / 1000;
            this.uri = Uri.EMPTY;
        }

        public String getName() {
            return this.name;
        }

        public long getModifiedTime() {
            return this.modifiedTime;
        }

        public void updateModifiedTime() {
            this.modifiedTime = System.currentTimeMillis() / 1000;
        }

        public boolean isDirectory() {
            return this.mimeType.equals(DocumentsContract.Document.MIME_TYPE_DIR);
        }

        Uri getUri() {
            if (baseUri != Uri.EMPTY && this.uri == Uri.EMPTY) {
                this.uri = DocumentsContract.buildDocumentUriUsingTree(baseUri, this.documentId);
            }
            return this.uri;
        }

        Uri getParentUri() {
            return this.parent;
        }
    }
}